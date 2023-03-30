[BaseContainerProps()]
class EL_EntitySaveDataClass
{
	[Attribute(desc: "Components to persist."), NonSerialized()]
	ref array<ref EL_ComponentSaveDataClass> m_aComponents;
}

class EL_EntitySaveData : EL_MetaDataDbEntity
{
	ResourceName m_rPrefab;
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
		ReadMetaData(EL_ComponentFinder<EL_PersistenceComponent>.Find(worldEntity));

		m_rPrefab = EL_Utils.GetPrefabName(worldEntity);

		m_mComponentsSaveData = new map<typename, ref array<ref EL_ComponentSaveData>>();

		array<Managed> processedComponents();

		// Go through hierarchy sorted component types
		foreach (EL_ComponentSaveDataClass componentSaveDataClass : attributes.m_aComponents)
		{
			array<ref EL_ComponentSaveData> componentsSaveData();

			typename saveDataType = EL_Utils.TrimEnd(componentSaveDataClass.ClassName(), 5).ToType();
			if (!saveDataType) return false;

			// Special handling for transformation as its not really a "component"
			if (componentSaveDataClass.IsInherited(EL_TransformationSaveDataClass))
			{
				EL_TransformationSaveData transformationSaveData();
				if (!transformationSaveData.ReadFrom(worldEntity, componentSaveDataClass)) return false;
				componentsSaveData.Insert(transformationSaveData);
			}
			else
			{
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
		ApplyMetaData(EL_ComponentFinder<EL_PersistenceComponent>.Find(worldEntity));

		set<Managed> processedComponents();
		set<typename> processedSaveDataTypes();

		foreach (EL_ComponentSaveDataClass componentSaveDataClass : attributes.m_aComponents)
		{
			// Special handling for transformation as its not really a "component"
			if (componentSaveDataClass.IsInherited(EL_TransformationSaveDataClass))
			{
				array<ref EL_ComponentSaveData> transformData = m_mComponentsSaveData.Get(EL_TransformationSaveData);
				if (transformData && transformData.Count() > 0) EL_TransformationSaveData.Cast(transformData.Get(0)).ApplyTo(worldEntity, componentSaveDataClass);
				processedSaveDataTypes.Insert(EL_TransformationSaveData);
				continue;
			}

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

		string prefabString = m_rPrefab;
		if (prefabString.StartsWith("{")) prefabString = m_rPrefab.Substring(1, 16);
		saveContext.WriteValue("m_rPrefab", prefabString);

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

		loadContext.ReadValue("m_rPrefab", m_rPrefab);
		if (m_rPrefab) m_rPrefab = string.Format("{%1}", m_rPrefab);

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
