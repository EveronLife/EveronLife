enum EL_ETransformSaveFlags
{
	COORDS = 0x01,
	ANGLES = 0x02,
	SCALE = 0x04
}

[BaseContainerProps()]
class EL_EntitySaveDataClass
{
	[Attribute("3", UIWidgets.Flags, desc: "Choose which aspects from the entity transformation are persisted.", enums: ParamEnumArray.FromEnum(EL_ETransformSaveFlags))]
	EL_ETransformSaveFlags m_eTranformSaveFlags;

	[Attribute("1", desc: "Only relevant if the world contains an active GarbageManager.")]
	bool m_bSaveRemainingLifetime;

	[Attribute(desc: "Components to persist.")]
	ref array<ref EL_ComponentSaveDataClass> m_aComponents;
}

class EL_EntitySaveData : EL_MetaDataDbEntity
{
	ResourceName m_rPrefab;
	ref EL_PersistentTransformation m_pTransformation;
	float m_fRemainingLifetime;
	ref map<typename, ref array<ref EL_ComponentSaveData>> m_mComponentsSaveData;

	//------------------------------------------------------------------------------------------------
	//! Spawn the world entity based on this save-data instance
	//! \return world entity or null if it could not be correctly spawned/loaded
	IEntity Spawn()
	{
		return EL_PersistenceManager.GetInstance().SpawnWorldEntity(this);
	}

	//------------------------------------------------------------------------------------------------
	//! Reads the save-data from the world entity
	//! \param worldEntity the entity to read the save-data from
	//! \param attributes the class-class shared configuration attributes assigned in the world editor
	//! \return true if save-data could be read, false if something failed.
	bool ReadFrom(notnull IEntity worldEntity, notnull EL_EntitySaveDataClass attributes)
	{
		ReadMetaData(EL_Component<EL_PersistenceComponent>.Find(worldEntity));

		// Prefab
		m_rPrefab = EL_Utils.GetPrefabName(worldEntity);

		// Transform
		m_pTransformation = new EL_PersistentTransformation();
		if (attributes.m_eTranformSaveFlags & EL_ETransformSaveFlags.COORDS)
		{
			m_pTransformation.m_vOrigin = worldEntity.GetOrigin();
		}
		if (attributes.m_eTranformSaveFlags & EL_ETransformSaveFlags.ANGLES)
		{
			m_pTransformation.m_vAngles = worldEntity.GetLocalYawPitchRoll();
		}
		if (attributes.m_eTranformSaveFlags & EL_ETransformSaveFlags.SCALE)
		{
			m_pTransformation.m_fScale = worldEntity.GetScale();
		}

		// Lifetime
		if (attributes.m_bSaveRemainingLifetime)
		{
			GarbageManager garbageManager = GetGame().GetGarbageManager();
			if (garbageManager) m_fRemainingLifetime = garbageManager.GetRemainingLifetime(worldEntity);
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
			worldEntity.FindComponents(EL_ComponentSaveDataType.Get(componentSaveDataClass.Type()), outComponents);
			foreach (Managed componentRef : outComponents)
			{
				// Ingore base class find machtes if a parent class was already processed
				if (processedComponents.Contains(componentRef)) continue;
				processedComponents.Insert(componentRef);

				EL_ComponentSaveData componentSaveData = EL_ComponentSaveData.Cast(saveDataType.Spawn());
				if (!componentSaveData || !componentSaveData.ReadFrom(GenericComponent.Cast(componentRef), componentSaveDataClass)) return false;

				componentsSaveData.Insert(componentSaveData);
			}

			if (componentsSaveData.Count() > 0)
			{
				m_mComponentsSaveData.Set(saveDataType, componentsSaveData);
			}
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Applies the save-data a world entity
	//! \param worldEntity the entity to apply the data to
	//! \param attributes the class-class shared configuration attributes assigned in the world editor
	//! \return true if save-data could be applied, false if something failed.
	bool ApplyTo(notnull IEntity worldEntity, notnull EL_EntitySaveDataClass attributes)
	{
		// Transform
		if (m_pTransformation)
		{
			EL_Utils.ForceTransform(worldEntity, m_pTransformation.m_vOrigin, m_pTransformation.m_vAngles, m_pTransformation.m_fScale);
		}

		// Lifetime
		if (attributes.m_bSaveRemainingLifetime)
		{
			GarbageManager garbageManager = GetGame().GetGarbageManager();
			if (garbageManager && m_fRemainingLifetime > 0) garbageManager.Insert(worldEntity, m_fRemainingLifetime);
		}

		// Components
		set<Managed> processedComponents();
		set<typename> processedSaveDataTypes();

		foreach (EL_ComponentSaveDataClass componentSaveDataClass : attributes.m_aComponents)
		{
			if (!ApplyComponent(componentSaveDataClass, worldEntity, processedSaveDataTypes, processedComponents, attributes)) return false;
		}

		// Update any non character entity. On character this can cause fall through ground.
		if (!ChimeraCharacter.Cast(worldEntity)) worldEntity.Update();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool ApplyComponent(
		EL_ComponentSaveDataClass componentSaveDataClass,
		IEntity worldEntity,
		set<typename> processedSaveDataTypes,
		set<Managed> processedComponents,
		EL_EntitySaveDataClass attributes)
	{
		typename componentSaveDataType = EL_Utils.TrimEnd(componentSaveDataClass.ClassName(), 5).ToType();

		// Skip already processed save-data
		if (processedSaveDataTypes.Contains(componentSaveDataType)) return true;
		processedSaveDataTypes.Insert(componentSaveDataType);

		// Make sure required save-data is already applied
		array<ref EL_ComponentSaveData> componentsSaveData = m_mComponentsSaveData.Get(componentSaveDataType);
		if (!componentsSaveData || componentsSaveData.IsEmpty()) return true;

		array<typename> requiredSaveDataClasses = componentSaveDataClass.Requires();
		if (requiredSaveDataClasses)
		{
			foreach (typename requiredSaveDataClass : requiredSaveDataClasses)
			{
				foreach (EL_ComponentSaveDataClass possibleComponentClass : attributes.m_aComponents)
				{
					if (possibleComponentClass == componentSaveDataClass ||
						!possibleComponentClass.IsInherited(requiredSaveDataClass)) continue;

					ApplyComponent(possibleComponentClass, worldEntity, processedSaveDataTypes, processedComponents, attributes);
				}
			}
		}

		// Apply save-data to matching components
		array<Managed> outComponents();
		worldEntity.FindComponents(EL_ComponentSaveDataType.Get(componentSaveDataClass.Type()), outComponents);
		foreach (EL_ComponentSaveData componentSaveData : componentsSaveData)
		{
			bool applied = false;

			foreach (Managed componentRef : outComponents)
			{
				if (!processedComponents.Contains(componentRef) && componentSaveData.IsFor(GenericComponent.Cast(componentRef), componentSaveDataClass))
				{
					if (!componentSaveData.ApplyTo(GenericComponent.Cast(componentRef), componentSaveDataClass)) return false;
					processedComponents.Insert(componentRef);
					applied = true;
					break;
				}
			}

			if (!applied)
			{
				Print(string.Format("No matching component for '%1' found on entity '%2'@%3", componentSaveData.Type().ToString(), EL_Utils.GetPrefabName(worldEntity), worldEntity.GetOrigin()), LogLevel.VERBOSE);
			}
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

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
		if (m_fRemainingLifetime > 0 || ContainerSerializationSaveContext.Cast(saveContext).GetContainer().IsInherited(BinSaveContainer))
		{
			saveContext.WriteValue("m_fRemainingLifetime", m_fRemainingLifetime);
		}

		// Components
		array<ref EL_PersistentComponentSaveData> componentSaveDataWrapper();
		foreach (auto _, array<ref EL_ComponentSaveData> componentsSaveData : m_mComponentsSaveData)
		{
			foreach (EL_ComponentSaveData component : componentsSaveData)
			{
				EL_PersistentComponentSaveData container();
				container.m_pData = component;
				componentSaveDataWrapper.Insert(container);
			}
		}

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
		#ifndef PERSISTENCE_DEBUG
		if (m_rPrefab) m_rPrefab = string.Format("{%1}", m_rPrefab);
		#endif

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
}

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
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		// For binary stream the info which of the 3 possible props will be written after needs to be known.
		// JSON just has the keys or not, so there it is not a problem.
		EL_ETransformSaveFlags flags;
		if (m_vOrigin != EL_Const.VEC_INFINITY) flags |= EL_ETransformSaveFlags.COORDS;
		if (m_vAngles != EL_Const.VEC_INFINITY) flags |= EL_ETransformSaveFlags.ANGLES;
		if (m_fScale != float.INFINITY) flags |= EL_ETransformSaveFlags.SCALE;

		if (ContainerSerializationSaveContext.Cast(saveContext).GetContainer().IsInherited(BinSaveContainer))
		{
			saveContext.WriteValue("transformSaveFlags", flags);
		}

		if (flags & EL_ETransformSaveFlags.COORDS) saveContext.WriteValue("m_vOrigin", m_vOrigin);
		if (flags & EL_ETransformSaveFlags.ANGLES) saveContext.WriteValue("m_vAngles", m_vAngles);
		if (flags & EL_ETransformSaveFlags.SCALE) saveContext.WriteValue("m_fScale", m_fScale);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		EL_ETransformSaveFlags flags = EL_ETransformSaveFlags.COORDS | EL_ETransformSaveFlags.ANGLES | EL_ETransformSaveFlags.SCALE;
		if (ContainerSerializationLoadContext.Cast(loadContext).GetContainer().IsInherited(BinLoadContainer))
		{
			loadContext.ReadValue("transformSaveFlags", flags);
		}

		if (flags & EL_ETransformSaveFlags.COORDS) loadContext.ReadValue("m_vOrigin", m_vOrigin);
		if (flags & EL_ETransformSaveFlags.ANGLES) loadContext.ReadValue("m_vAngles", m_vAngles);
		if (flags & EL_ETransformSaveFlags.SCALE) loadContext.ReadValue("m_fScale", m_fScale);

		return true;
	}

	void EL_PersistentTransformation()
	{
		Reset();
	}
}

class EL_PersistentComponentSaveData
{
	ref EL_ComponentSaveData m_pData;

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		saveContext.WriteValue("dataType", EL_DbName.Get(m_pData.Type()));
		saveContext.WriteValue("m_pData", m_pData);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		string dataTypeString;
		loadContext.ReadValue("dataType", dataTypeString);
		typename dataType = EL_DbName.GetTypeByName(dataTypeString);
		if (!dataType) return false;

		m_pData = EL_ComponentSaveData.Cast(dataType.Spawn());
		loadContext.ReadValue("m_pData", m_pData);

		return true;
	}
}

class EL_ComponentSaveDataGetter<Class T>
{
	//------------------------------------------------------------------------------------------------
	static T GetFirst(EL_EntitySaveData saveData)
	{
		if (!saveData) return null;
		auto componentsSaveData = saveData.m_mComponentsSaveData.Get(T);
		if (!componentsSaveData || componentsSaveData.IsEmpty()) return null;
		return T.Cast(componentsSaveData[0]);
	}	
}
