[EL_DbName(EL_EntitySaveDataBase, "Entity"), BaseContainerProps()]
class EL_EntitySaveDataBase : EL_DbEntity
{
	[Attribute(desc: "Sava-data types for components to persist."), NonSerialized()]
	ref array<ref EL_ComponentSaveDataBase> m_aComponents;

	int m_iDataLayoutVersion = 1;
	EL_DateTimeUtcAsInt m_iLastSaved;

	ResourceName m_rPrefab;
	ref map<typename, ref array<ref EL_ComponentSaveDataBase>> m_mComponentsSaveData;

	//------------------------------------------------------------------------------------------------
	//! Reads the save-data from the world entity
	//! \param worldEntity the entity to read the save-data from
	//! \return true if save-data could be read, false if something failed.
	bool ReadFrom(notnull IEntity worldEntity)
	{
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(worldEntity));

		SetId(persistenceComponent.GetPersistentId());
		m_iLastSaved = persistenceComponent.GetLastSaved();

		m_rPrefab = EL_Utils.GetPrefabName(worldEntity);

		m_mComponentsSaveData = new map<typename, ref array<ref EL_ComponentSaveDataBase>>();

		// Go through hierarchy sorted component types
		foreach (typename componentSaveDataType : settings.m_aComponentSaveDataTypenames)
		{
			array<ref EL_ComponentSaveDataBase> componentsSaveData();

			// Special handling for transformation as its not really a "component"
			if (componentSaveDataType == EL_TransformationSaveData)
			{
				EL_TransformationSaveData transformationSaveData = EL_TransformationSaveData.Cast(componentSaveDataType.Spawn());
				if (!transformationSaveData || !transformationSaveData.ReadFrom(worldEntity)) return false;

				componentsSaveData.Insert(transformationSaveData);
			}
			else
			{
				array<Managed> outComponents();
				worldEntity.FindComponents(EL_ComponentSaveDataType.Get(componentSaveDataType), outComponents);
				foreach (Managed componentRef : outComponents)
				{
					typename componentType = componentRef.Type();

					// Ingore base class find machtes if the parent class was already processed
					if (m_mComponentsSaveData.Contains(componentType)) continue;

					EL_ComponentSaveDataBase componentSaveData = EL_ComponentSaveDataBase.Cast(componentSaveDataType.Spawn());
					if (!componentSaveData || !componentSaveData.ReadFrom(GenericComponent.Cast(componentRef))) return false;

					componentsSaveData.Insert(componentSaveData);
				}
			}

			if (componentsSaveData.Count() > 0)
			{
				m_mComponentsSaveData.Set(componentSaveDataType, componentsSaveData);
			}
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Spawn the world entity based on this save-data instance
	//! \return world entity or null if it could not be correctly spawned/loaded
	IEntity Spawn()
	{
		return EL_PersistenceManager.GetInstance().SpawnWorldEntity(this);
	}

	//------------------------------------------------------------------------------------------------
	//! Applies the save-data a world entity
	//! \param worldEntity the entity to apply the data to
	//! \return true if save-data could be applied, false if something failed.
	bool ApplyTo(notnull IEntity worldEntity)
	{
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(worldEntity));

		// Special handling for transformation as its not really a "component" and it helps other component loads if the entity position is already set.
		array<ref EL_ComponentSaveDataBase> transformData = m_mComponentsSaveData.Get(EL_TransformationSaveData);
		if (transformData && transformData.Count() > 0) EL_TransformationSaveData.Cast(transformData.Get(0)).ApplyTo(worldEntity);

		set<Managed> processedInstances();
		set<typename> processedSaveDataTypes();
		processedSaveDataTypes.Insert(EL_TransformationSaveData);
		foreach (typename componentSaveDataType : settings.m_aComponentSaveDataTypenames)
		{
			if (!ApplyComponentTo(componentSaveDataType, worldEntity, processedSaveDataTypes, processedInstances, settings)) return false;
		}

		// Update any non character entity. On character this can cause fall through ground.
		if (!ChimeraCharacter.Cast(worldEntity)) worldEntity.Update();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool ApplyComponentTo(typename componentSaveDataType, IEntity worldEntity, set<typename> processedSaveDataTypes, set<Managed> processedInstances, EL_PersistenceComponentClass settings)
	{
		// Skip already processed save-data
		if (processedSaveDataTypes.Contains(componentSaveDataType)) return true;
		processedSaveDataTypes.Insert(componentSaveDataType);

		// Make sure required save-data is already applied
		array<ref EL_ComponentSaveDataBase> componentsSaveData = m_mComponentsSaveData.Get(componentSaveDataType);
		if (!componentsSaveData || componentsSaveData.IsEmpty()) return true;

		array<typename> requiredSaveDataTypes = componentsSaveData.Get(0).Requires();
		if (requiredSaveDataTypes)
		{
			foreach (typename requiredSaveDataType : requiredSaveDataTypes)
			{
				foreach (typename possibleType : settings.m_aComponentSaveDataTypenames)
				{
					if (!possibleType.IsInherited(requiredSaveDataType)) continue;

					ApplyComponentTo(possibleType, worldEntity, processedSaveDataTypes, processedInstances, settings);
				}
			}
		}

		// Apply save-data to matching components
		array<Managed> outComponents();
		worldEntity.FindComponents(EL_ComponentSaveDataType.Get(componentSaveDataType), outComponents);
		foreach (EL_ComponentSaveDataBase componentSaveData : componentsSaveData)
		{
			bool applied = false;

			foreach (Managed componentRef : outComponents)
			{
				if (!processedInstances.Contains(componentRef) && componentSaveData.IsFor(GenericComponent.Cast(componentRef)))
				{
					if (!componentSaveData.ApplyTo(GenericComponent.Cast(componentRef))) return false;
					processedInstances.Insert(componentRef);
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

		saveContext.WriteValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		saveContext.WriteValue("m_sId", GetId());
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);
		
		string prefabString = m_rPrefab;
		if (prefabString.StartsWith("{")) prefabString = m_rPrefab.Substring(1, m_rPrefab.IndexOf("}") - 1);
		saveContext.WriteValue("m_rPrefab", prefabString);

		array<ref EL_PersistentComponentSaveData> componentSaveDataWrapper();
		foreach (auto _, array<ref EL_ComponentSaveDataBase> componentsSaveData : m_mComponentsSaveData)
		{
			foreach (EL_ComponentSaveDataBase component : componentsSaveData)
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

		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);

		string id;
		loadContext.ReadValue("m_sId", id);
		SetId(id);

		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);

		loadContext.ReadValue("m_rPrefab", m_rPrefab);
		if (m_rPrefab) m_rPrefab = string.Format("{%1}", m_rPrefab);

		m_mComponentsSaveData = new map<typename, ref array<ref EL_ComponentSaveDataBase>>();

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
	ref EL_ComponentSaveDataBase m_pData;

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

		m_pData = EL_ComponentSaveDataBase.Cast(dataType.Spawn());
		loadContext.ReadValue("m_pData", m_pData);

		return true;
	}
}
