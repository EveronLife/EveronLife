enum EL_ETransformSaveFlags
{
	COORDS = 0x01,
	ANGLES = 0x02,
	SCALE = 0x04
};

[BaseContainerProps()]
class EL_EntitySaveDataClass
{
	[Attribute("1", desc: "Trim default values from the save data to minimize storage and avoid default value entires in the database.\nE.g. there is no need to persist that a cars engine is off.")]
	bool m_bTrimDefaults;

	[Attribute("3", UIWidgets.Flags, desc: "Choose which aspects from the entity transformation are persisted.", enums: ParamEnumArray.FromEnum(EL_ETransformSaveFlags))]
	EL_ETransformSaveFlags m_eTranformSaveFlags;

	[Attribute("1", desc: "Only relevant if the world contains an active GarbageManager.")]
	bool m_bSaveRemainingLifetime;

	[Attribute(desc: "Components to persist.")]
	ref array<ref EL_ComponentSaveDataClass> m_aComponents;
};

class EL_EntitySaveData : EL_MetaDataDbEntity
{
	ResourceName m_rPrefab;
	ref EL_PersistentTransformation m_pTransformation;
	float m_fRemainingLifetime;
	ref map<typename, ref array<ref EL_ComponentSaveData>> m_mComponentsSaveData;

	//------------------------------------------------------------------------------------------------
	//! Spawn the world entity based on this save-data instance
	//! \param isRoot true if the current entity is a world root (not a stored item inside a storage)
	//! \return world entity or null if it could not be correctly spawned/loaded
	IEntity Spawn(bool isRoot = true)
	{
		return EL_PersistenceManager.GetInstance().SpawnWorldEntity(this, isRoot);
	}

	//------------------------------------------------------------------------------------------------
	//! Reads the save-data from the world entity
	//! \param entity to read the save-data from
	//! \param attributes the class-class shared configuration attributes assigned in the world editor
	//! \return EL_EReadResult.OK if save-data could be read, ERROR if something failed, NODATA for all default values
	EL_EReadResult ReadFrom(IEntity entity, EL_EntitySaveDataClass attributes)
	{
		EL_EReadResult resultCode = EL_EReadResult.DEFAULT;
		if (!attributes.m_bTrimDefaults) resultCode = EL_EReadResult.OK;

		EL_PersistenceComponent persistenceComponent = EL_Component<EL_PersistenceComponent>.Find(entity);
		ReadMetaData(persistenceComponent);

		// Prefab
		m_rPrefab = EL_Utils.GetPrefabName(entity);

		// Transform
		m_pTransformation = new EL_PersistentTransformation();
		// We save it on root entities and always on characters (in case the parent vehicle is not loaded back in)
		// We can skip transform for baked entities that were not moved.
		EL_EPersistenceFlags flags = persistenceComponent.GetFlags();
		if (EL_BitFlags.CheckFlags(flags, EL_EPersistenceFlags.ROOT) &&
			(!EL_BitFlags.CheckFlags(flags, EL_EPersistenceFlags.BAKED) || EL_BitFlags.CheckFlags(flags, EL_EPersistenceFlags.WAS_MOVED)))
		{
			if (m_pTransformation.ReadFrom(entity, attributes))
				resultCode = EL_EReadResult.OK;
		}

		// Lifetime
		if (EL_BitFlags.CheckFlags(flags, EL_EPersistenceFlags.ROOT) &&
			attributes.m_bSaveRemainingLifetime)
		{
			GarbageManager garbageManager = GetGame().GetGarbageManager();
			if (garbageManager)
				m_fRemainingLifetime = garbageManager.GetRemainingLifetime(entity);

			if (m_fRemainingLifetime == -1)
			{
				m_fRemainingLifetime = 0;
			}
			else if (m_fRemainingLifetime > 0)
			{
				resultCode = EL_EReadResult.OK;
			}
		}

		// Components
		m_mComponentsSaveData = new map<typename, ref array<ref EL_ComponentSaveData>>();

		array<Managed> processedComponents();

		// Go through hierarchy sorted component types
		foreach (EL_ComponentSaveDataClass componentSaveDataClass : attributes.m_aComponents)
		{
			array<ref EL_ComponentSaveData> componentsSaveData();

			typename saveDataType = EL_Utils.TrimEnd(componentSaveDataClass.ClassName(), 5).ToType();
			if (!saveDataType) return false;

			array<Managed> outComponents();
			entity.FindComponents(EL_ComponentSaveDataType.Get(componentSaveDataClass.Type()), outComponents);
			foreach (Managed componentRef : outComponents)
			{
				// Ingore base class find machtes if a parent class was already processed
				if (processedComponents.Contains(componentRef)) continue;
				processedComponents.Insert(componentRef);

				EL_ComponentSaveData componentSaveData = EL_ComponentSaveData.Cast(saveDataType.Spawn());
				if (!componentSaveData) return EL_EReadResult.ERROR;

				componentSaveDataClass.m_bTrimDefaults = attributes.m_bTrimDefaults;
				EL_EReadResult componentRead = componentSaveData.ReadFrom(entity, GenericComponent.Cast(componentRef), componentSaveDataClass);
				if (componentRead == EL_EReadResult.ERROR) return componentRead;
				if (componentRead == EL_EReadResult.DEFAULT && attributes.m_bTrimDefaults) continue;

				componentsSaveData.Insert(componentSaveData);
			}

			if (componentsSaveData.Count() > 0)
			{
				m_mComponentsSaveData.Set(saveDataType, componentsSaveData);
				resultCode = EL_EReadResult.OK;
			}
		}

		return resultCode;
	}

	//------------------------------------------------------------------------------------------------
	//! Applies the save-data a world entity
	//! \param entity to apply the data to
	//! \param attributes the class-class shared configuration attributes assigned in the world editor
	//! \return true if save-data could be applied, false if something failed.
	EL_EApplyResult ApplyTo(IEntity entity, EL_EntitySaveDataClass attributes)
	{
		EL_EApplyResult result = EL_EApplyResult.OK;

		// Transform
		if (m_pTransformation && !m_pTransformation.IsDefault())
			EL_Utils.ForceTransform(entity, m_pTransformation.m_vOrigin, m_pTransformation.m_vAngles, m_pTransformation.m_fScale);

		// Lifetime
		if (attributes.m_bSaveRemainingLifetime)
		{
			GarbageManager garbageManager = GetGame().GetGarbageManager();
			if (garbageManager && m_fRemainingLifetime > 0)
				garbageManager.Insert(entity, m_fRemainingLifetime);
		}

		// Components
		set<Managed> processedComponents();
		set<typename> processedSaveDataTypes();
		foreach (EL_ComponentSaveDataClass componentSaveDataClass : attributes.m_aComponents)
		{
			EL_EApplyResult componentResult = ApplyComponent(componentSaveDataClass, entity, processedSaveDataTypes, processedComponents, attributes);

			if (componentResult == EL_EApplyResult.ERROR)
				return EL_EApplyResult.ERROR;

			if (componentResult == EL_EApplyResult.AWAIT_COMPLETION)
				result = EL_EApplyResult.AWAIT_COMPLETION;
		}

		// Update any non character entity. On character this can cause fall through ground.
		if (!ChimeraCharacter.Cast(entity))
			entity.Update();

		return result;
	}

	//------------------------------------------------------------------------------------------------
	//! Compare entity save-data instances to see if there is any noteable difference
	//! \param other entity save-data to compare against
	//! \return true if save-data is considered to describe the same data. False on differences.
	bool Equals(notnull EL_EntitySaveData other)
	{
		// Same prefab?
		if (m_rPrefab != other.m_rPrefab)
			return false;

		// Same transformation?
		if ((vector.Distance(m_pTransformation.m_vOrigin, other.m_pTransformation.m_vOrigin) > 0.0001) ||
			(vector.Distance(m_pTransformation.m_vAngles, other.m_pTransformation.m_vAngles) > 0.0001) ||
			((m_pTransformation.m_fScale != float.INFINITY || other.m_pTransformation.m_fScale != float.INFINITY) &&
				!float.AlmostEqual(m_pTransformation.m_fScale, other.m_pTransformation.m_fScale)))
		{
			return false;
		}

		// Same lifetime?
		if (m_fRemainingLifetime != other.m_fRemainingLifetime)
			return false;

		// See if we can match all component save-data instances
		foreach (typename saveDataType, array<ref EL_ComponentSaveData> components : m_mComponentsSaveData)
		{
			array<ref EL_ComponentSaveData> otherComponents = other.m_mComponentsSaveData.Get(saveDataType);
			if (!otherComponents || otherComponents.Count() != components.Count())
				return false;

			foreach (int idx, EL_ComponentSaveData componentSaveData : components)
			{
				// Try same index first as they are likely to be the correct ones.
				if (componentSaveData.Equals(otherComponents.Get(idx)))
					continue;

				bool found;
				foreach (int compareIdx, EL_ComponentSaveData otherComponent : otherComponents)
				{
					if (compareIdx == idx)
						continue; // Already tried in idx direct compare

					if (componentSaveData.Equals(otherComponent))
					{
						found = true;
						break;
					}
				}

				if (!found)
					return false; //Unable to find any matching component save-data
			}
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected EL_EApplyResult ApplyComponent(
		EL_ComponentSaveDataClass componentSaveDataClass,
		IEntity entity,
		set<typename> processedSaveDataTypes,
		set<Managed> processedComponents,
		EL_EntitySaveDataClass attributes)
	{
		EL_EApplyResult result = EL_EApplyResult.OK;

		typename componentSaveDataType = EL_Utils.TrimEnd(componentSaveDataClass.ClassName(), 5).ToType();

		// Skip already processed save-data
		if (processedSaveDataTypes.Contains(componentSaveDataType)) return result;
		processedSaveDataTypes.Insert(componentSaveDataType);

		// Make sure required save-data is already applied
		array<ref EL_ComponentSaveData> componentsSaveData = m_mComponentsSaveData.Get(componentSaveDataType);
		if (!componentsSaveData || componentsSaveData.IsEmpty()) return result;

		array<typename> requiredSaveDataClasses = componentSaveDataClass.Requires();
		if (requiredSaveDataClasses)
		{
			foreach (typename requiredSaveDataClass : requiredSaveDataClasses)
			{
				if (!requiredSaveDataClass.ToString().EndsWith("Class"))
				{
					Debug.Error(string.Format("Save-data class '%1' lists invalid (non xyzClass) requirement type '%2'. Fix or remove it.", componentSaveDataClass.Type().ToString(), requiredSaveDataClass));
					return EL_EApplyResult.ERROR;
				}

				foreach (EL_ComponentSaveDataClass possibleComponentClass : attributes.m_aComponents)
				{
					if (possibleComponentClass == componentSaveDataClass ||
						!possibleComponentClass.IsInherited(requiredSaveDataClass)) continue;

					EL_EApplyResult componentResult = ApplyComponent(possibleComponentClass, entity, processedSaveDataTypes, processedComponents, attributes);

					if (componentResult == EL_EApplyResult.ERROR)
						return EL_EApplyResult.ERROR;

					if (componentResult == EL_EApplyResult.AWAIT_COMPLETION)
						result = EL_EApplyResult.AWAIT_COMPLETION;
				}
			}
		}

		// Apply save-data to matching components
		array<Managed> outComponents();
		entity.FindComponents(EL_ComponentSaveDataType.Get(componentSaveDataClass.Type()), outComponents);
		foreach (EL_ComponentSaveData componentSaveData : componentsSaveData)
		{
			bool applied = false;
			foreach (Managed componentRef : outComponents)
			{
				if (!processedComponents.Contains(componentRef) && componentSaveData.IsFor(entity, GenericComponent.Cast(componentRef), componentSaveDataClass))
				{
					EL_EApplyResult componentResult = componentSaveData.ApplyTo(entity, GenericComponent.Cast(componentRef), componentSaveDataClass);

					if (componentResult == EL_EApplyResult.ERROR)
						return EL_EApplyResult.ERROR;

					if (componentResult == EL_EApplyResult.AWAIT_COMPLETION &&
						EL_DeferredApplyResult.SetEntitySaveData(componentSaveData, this))
					{
						result = EL_EApplyResult.AWAIT_COMPLETION;
					}

					processedComponents.Insert(componentRef);
					applied = true;
					break;
				}
			}

			if (!applied)
			{
				Print(string.Format("No matching component for '%1' found on entity '%2'@%3", componentSaveData.Type().ToString(), EL_Utils.GetPrefabName(entity), entity.GetOrigin()), LogLevel.VERBOSE);
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		bool isJson = ContainerSerializationSaveContext.Cast(saveContext).GetContainer().IsInherited(BaseJsonSerializationSaveContainer);
		
		SerializeMetaData(saveContext);

		// Prefab
		string prefabString = m_rPrefab;
		#ifndef PERSISTENCE_DEBUG
		if (prefabString.StartsWith("{")) prefabString = m_rPrefab.Substring(1, 16);
		#endif
		saveContext.WriteValue("m_rPrefab", prefabString);

		// Transform
		saveContext.WriteValue("m_pTransformation", m_pTransformation);

		// Lifetime
		if (m_fRemainingLifetime > 0 || !isJson)
			saveContext.WriteValue("m_fRemainingLifetime", m_fRemainingLifetime);

		// Components
		array<ref EL_PersistentComponentSaveData> componentSaveDataWrapper();
		foreach (typename type, array<ref EL_ComponentSaveData> componentsSaveData : m_mComponentsSaveData)
		{
			foreach (EL_ComponentSaveData component : componentsSaveData)
			{
				EL_PersistentComponentSaveData container();
				container.m_pData = component;
				componentSaveDataWrapper.Insert(container);
			}
		}

		if (!componentSaveDataWrapper.IsEmpty() || !isJson)
			saveContext.WriteValue("m_aComponents", componentSaveDataWrapper);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		DeserializeMetaData(loadContext);

		// Prefab
		loadContext.ReadValue("m_rPrefab", m_rPrefab);
		if (m_rPrefab && m_rPrefab[0] != "{") m_rPrefab = string.Format("{%1}", m_rPrefab);

		// Transform
		loadContext.ReadValue("m_pTransformation", m_pTransformation);

		// Lifetime
		loadContext.ReadValue("m_fRemainingLifetime", m_fRemainingLifetime);

		// Components
		m_mComponentsSaveData = new map<typename, ref array<ref EL_ComponentSaveData>>();

		array<ref EL_PersistentComponentSaveData> componentSaveDataWrapper();
		loadContext.ReadValue("m_aComponents", componentSaveDataWrapper);
		foreach (EL_PersistentComponentSaveData container : componentSaveDataWrapper)
		{
			typename componentSaveDataType = container.m_pData.Type();

			if (!m_mComponentsSaveData.Contains(componentSaveDataType))
			{
				m_mComponentsSaveData.Set(componentSaveDataType, {container.m_pData});
				continue;
			}

			m_mComponentsSaveData.Get(componentSaveDataType).Insert(container.m_pData);
		}

		return true;
	}
};

class EL_PersistentTransformation
{
	vector m_vOrigin;
	vector m_vAngles;
	float m_fScale;

	//------------------------------------------------------------------------------------------------
	void Reset()
	{
		m_vOrigin = EL_Const.VEC_INFINITY;
		m_vAngles = EL_Const.VEC_INFINITY;
		m_fScale = float.INFINITY;
	}

	//------------------------------------------------------------------------------------------------
	bool IsDefault()
	{
		return m_vOrigin == EL_Const.VEC_INFINITY &&
			m_vAngles == EL_Const.VEC_INFINITY &&
			m_fScale == float.INFINITY;
	}

	//------------------------------------------------------------------------------------------------
	bool ReadFrom(IEntity entity, EL_EntitySaveDataClass attributes)
	{
		bool anyData;

		vector angles, transform[4];
		float scale = entity.GetScale();

		// For chars (in vehicles) we want to keep the world transform
		// for if the parent vehicle is deleted they can still spawn
		if (!ChimeraCharacter.Cast(entity) && entity.GetParent())
		{
			entity.GetLocalTransform(transform);
			angles = entity.GetLocalYawPitchRoll();
		}
		else
		{
			entity.GetWorldTransform(transform);
			angles = entity.GetYawPitchRoll();
		}

		vector origin = transform[3];

		if ((attributes.m_eTranformSaveFlags & EL_ETransformSaveFlags.COORDS) &&
			(!attributes.m_bTrimDefaults || (origin != vector.Zero)))
		{
			m_vOrigin = origin;
			anyData = true;
		}

		if ((attributes.m_eTranformSaveFlags & EL_ETransformSaveFlags.ANGLES) &&
			(!attributes.m_bTrimDefaults || (angles != vector.Zero)))
		{
			m_vAngles = angles;
			anyData = true;
		}

		if ((attributes.m_eTranformSaveFlags & EL_ETransformSaveFlags.SCALE) &&
			(!attributes.m_bTrimDefaults || (scale != 1.0)))
		{
			m_fScale = scale;
			anyData = true;
		}

		return anyData;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		// For binary stream the info which of the 3 possible props will be written after needs to be known.
		// JSON just has the keys or not, so there it is not a problem.
		EL_ETransformSaveFlags flags;
		if (m_vOrigin != EL_Const.VEC_INFINITY)
			flags |= EL_ETransformSaveFlags.COORDS;

		if (m_vAngles != EL_Const.VEC_INFINITY)
			flags |= EL_ETransformSaveFlags.ANGLES;

		if (m_fScale != float.INFINITY)
			flags |= EL_ETransformSaveFlags.SCALE;

		if (ContainerSerializationSaveContext.Cast(saveContext).GetContainer().IsInherited(BinSaveContainer))
			saveContext.WriteValue("transformSaveFlags", flags);

		if (flags & EL_ETransformSaveFlags.COORDS)
			saveContext.WriteValue("m_vOrigin", m_vOrigin);

		if (flags & EL_ETransformSaveFlags.ANGLES)
			saveContext.WriteValue("m_vAngles", m_vAngles);

		if (flags & EL_ETransformSaveFlags.SCALE)
			saveContext.WriteValue("m_fScale", m_fScale);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		EL_ETransformSaveFlags flags = EL_ETransformSaveFlags.COORDS | EL_ETransformSaveFlags.ANGLES | EL_ETransformSaveFlags.SCALE;
		if (ContainerSerializationLoadContext.Cast(loadContext).GetContainer().IsInherited(BinLoadContainer))
			loadContext.ReadValue("transformSaveFlags", flags);

		if (flags & EL_ETransformSaveFlags.COORDS)
			loadContext.ReadValue("m_vOrigin", m_vOrigin);

		if (flags & EL_ETransformSaveFlags.ANGLES)
			loadContext.ReadValue("m_vAngles", m_vAngles);

		if (flags & EL_ETransformSaveFlags.SCALE)
			loadContext.ReadValue("m_fScale", m_fScale);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	void EL_PersistentTransformation()
	{
		Reset();
	}
};

class EL_PersistentComponentSaveData
{
	ref EL_ComponentSaveData m_pData;

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;

		saveContext.WriteValue("dataType", EL_DbName.Get(m_pData.Type()));
		saveContext.WriteValue("m_pData", m_pData);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid())
			return false;

		string dataTypeString;
		loadContext.ReadValue("dataType", dataTypeString);
		typename dataType = EL_DbName.GetTypeByName(dataTypeString);
		if (!dataType)
			return false;

		m_pData = EL_ComponentSaveData.Cast(dataType.Spawn());
		loadContext.ReadValue("m_pData", m_pData);

		return true;
	}
};

class EL_ComponentSaveDataGetter<Class T>
{
	//------------------------------------------------------------------------------------------------
	static T GetFirst(EL_EntitySaveData saveData)
	{
		if (!saveData)
			return null;

		array<ref EL_ComponentSaveData> componentsSaveData = saveData.m_mComponentsSaveData.Get(T);
		if (!componentsSaveData || componentsSaveData.IsEmpty())
			return null;

		return T.Cast(componentsSaveData[0]);
	}
};
