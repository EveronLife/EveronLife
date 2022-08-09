[BaseContainerProps()]
class EL_EntitySaveDataBase : EL_DbEntity
{
	[NonSerialized(), Attribute(desc: "Sava-data types for components to persist.")]
	ref array<ref EL_ComponentSaveDataBase> m_aComponents;
	
	DateTimeUtcAsInt m_iLastSaved;
	
	ResourceName m_Prefab;
	ref map<typename, ref array<ref EL_ComponentSaveDataBase>> m_aComponentsSaveData;
	
	bool ReadFrom(IEntity worldEntity)
	{
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(worldEntity));
		
		SetId(persistenceComponent.GetPersistentId());
		m_iLastSaved = persistenceComponent.GetLastSaved();
		
		m_Prefab = EL_Utils.GetPrefabName(worldEntity);
		
		m_aComponentsSaveData = new map<typename, ref array<ref EL_ComponentSaveDataBase>>();
		bool transformationHandled = false;
		
		// Go through hierarchy sorted component types
		foreach(typename componentSaveDataType : settings.m_aComponentSaveDataTypenames)
		{
			array<ref EL_ComponentSaveDataBase> componentsSaveData();
			
			// Special handling for transformation as its not really a "component"
			if(componentSaveDataType.IsInherited(EL_TransformationSaveData))
			{
				if(transformationHandled) return false;
				transformationHandled = true;
				
				EL_TransformationSaveData transformationSaveData = EL_TransformationSaveData.Cast(componentSaveDataType.Spawn());
				if(!transformationSaveData || !transformationSaveData.ReadFrom(worldEntity)) return false;
				
				componentsSaveData.Insert(transformationSaveData);
			}
			else
			{
				array<Managed> outComponents();
				worldEntity.FindComponents(EL_ComponentSaveDataType.Get(componentSaveDataType), outComponents);
				foreach(Managed componentRef : outComponents)
				{
					typename componentType = componentRef.Type();
					
					// Ingore base class find machtes if the parent class was already processed
					if(m_aComponentsSaveData.Contains(componentType)) continue;
					
					EL_ComponentSaveDataBase componentSaveData = EL_ComponentSaveDataBase.Cast(componentSaveDataType.Spawn());
					if(!componentSaveData || !componentSaveData.ReadFrom(GenericComponent.Cast(componentRef))) return false;
					
					componentsSaveData.Insert(componentSaveData);
				}
			}
			
			m_aComponentsSaveData.Set(componentSaveDataType, componentsSaveData);
		}
		
		return true;
	}
	
	IEntity Spawn()
	{
		return EL_PersistenceManager.GetInstance().SpawnWorldEntity(this);
	}
	
	bool ApplyTo(IEntity worldEntity)
	{
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		if(!persistenceComponent) return false;
		
		set<Managed> processed();
		foreach(typename saveDataType, array<ref EL_ComponentSaveDataBase> componentsSaveData : m_aComponentsSaveData)
		{
			//PrintFormat("Loading component type '%1' with %2 instances.", saveDataType, componentsSaveData.Count());
			
			// Special handling for transformation as its not really a "component"
			if(saveDataType.IsInherited(EL_TransformationSaveData))
			{
				EL_TransformationSaveData.Cast(componentsSaveData.Get(0)).ApplyTo(worldEntity);
				continue;
			}
			
			array<Managed> outComponents();
			worldEntity.FindComponents(EL_ComponentSaveDataType.Get(saveDataType), outComponents);
			
			foreach(EL_ComponentSaveDataBase componentSaveData : componentsSaveData)
			{
				bool applied = false;
				
				foreach(Managed componentRef : outComponents)
				{
					if(!processed.Contains(componentRef) && componentSaveData.IsFor(GenericComponent.Cast(componentRef)))
					{
						componentSaveData.ApplyTo(GenericComponent.Cast(componentRef));
						processed.Insert(componentRef);
						applied = true;
						continue;
					}
				}
				
				if(!applied)
				{
					Print(string.Format("No matching component for '%1' found on entity '%2'@%3", componentSaveData.Type(), EL_Utils.GetPrefabName(worldEntity), worldEntity.GetOrigin()), LogLevel.VERBOSE);
				}
			}
		}
		
		// Update any non character entity. On character this can cause fall through ground.
		if(!ChimeraCharacter.Cast(worldEntity))
		{
			worldEntity.Update();
		}
		
		return true;
	}
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("dataLayoutVersion", 1);
		saveContext.WriteValue("id", GetId());
		saveContext.WriteValue("dateTime", m_iLastSaved);
		saveContext.WriteValue("prefab", m_Prefab.Substring(1, m_Prefab.IndexOf("}") - 1));
		
		saveContext.StartObject("components");
		
		// Include components order info so they can be read back in the same order again (important for binary formats)
		array<string> componentTypesInOrder();
		array<EL_ComponentSaveDataBase> writeComponents();
	
		foreach(typename saveDataType, array<ref EL_ComponentSaveDataBase> componentsSaveData : m_aComponentsSaveData)
		{
			// If the same component type is included multiple times its written mutiple times
			foreach(EL_ComponentSaveDataBase component : componentsSaveData)
			{
				componentTypesInOrder.Insert(EL_DbName.Get(saveDataType));
				writeComponents.Insert(component);
			}
		}
		
		// Order needs to be written first for binary file types
		saveContext.WriteValue("order", componentTypesInOrder);
		
		// Then component data block can be written
		foreach(int idx, EL_ComponentSaveDataBase component : writeComponents)
		{
			saveContext.WriteValue(idx.ToString(), component);
		}
		
		saveContext.EndObject();
		
		return true;
	}

	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;
		
		int dataLayoutVersion;
		loadContext.ReadValue("dataLayoutVersion", dataLayoutVersion);
		
		string id;
		loadContext.ReadValue("id", id);
		SetId(id);
		
		loadContext.ReadValue("dateTime", m_iLastSaved);
		
		loadContext.ReadValue("prefab", m_Prefab);
		m_Prefab = string.Format("{%1}", m_Prefab);
		
		loadContext.StartObject("components");
		
		array<string> componentNamesInOrder;
		loadContext.ReadValue("order", componentNamesInOrder);
		
		m_aComponentsSaveData = new map<typename, ref array<ref EL_ComponentSaveDataBase>>();
		
		foreach(int idx, string componentName : componentNamesInOrder)
		{
			typename componentType = EL_DbName.GetTypeByName(componentName);
			EL_ComponentSaveDataBase componentSaveData = EL_ComponentSaveDataBase.Cast(componentType.Spawn());
			loadContext.ReadValue(idx.ToString(), componentSaveData);
			
			if(!m_aComponentsSaveData.Contains(componentType))
			{
				m_aComponentsSaveData.Set(componentType, {componentSaveData});
				continue;
			}
			
			m_aComponentsSaveData.Get(componentType).Insert(componentSaveData);
		}
		
		loadContext.EndObject();
		
		return true;
	}
}
