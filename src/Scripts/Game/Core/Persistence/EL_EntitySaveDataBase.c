[BaseContainerProps()]
class EL_EntitySaveDataBase : EL_DbEntity
{
	[NonSerialized(), Attribute(desc: "Sava-data types for components to persist.")]
	ref array<ref EL_ComponentSaveDataBase> m_aComponents;
	
	ResourceName m_Prefab;
	ref array<ref EL_TComponentDataTuple> m_aComponentsSaveData;
	
	bool ReadFrom(IEntity worldEntity)
	{
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(worldEntity));
		
		SetId(persistenceComponent.GetPersistentId());
		
		m_Prefab = EL_Utils.GetPrefabName(worldEntity);
		
		m_aComponentsSaveData = new array<ref EL_TComponentDataTuple>();
		bool transformationHandled = false;
		
		set<typename> proccessed();
		
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
					if(proccessed.Contains(componentType)) continue;
					
					EL_ComponentSaveDataBase componentSaveData = EL_ComponentSaveDataBase.Cast(componentSaveDataType.Spawn());
					if(!componentSaveData || !componentSaveData.ReadFrom(GenericComponent.Cast(componentRef))) return false;
					
					componentsSaveData.Insert(componentSaveData);
				}
			}
			
			m_aComponentsSaveData.Insert(new EL_TComponentDataTuple(componentSaveDataType, componentsSaveData));
			proccessed.Insert(componentSaveDataType);
		}
		
		return true;
	}

	bool ApplyTo(IEntity worldEntity)
	{
		set<Managed> processed()
		foreach(EL_TComponentDataTuple componentTuple : m_aComponentsSaveData)
		{
			PrintFormat("Loading component type '%1' with %2 instances.", componentTuple.m_tType, componentTuple.m_aSaveData.Count());
			
			// Special handling for transformation as its not really a "component"
			if(componentTuple.m_tType.IsInherited(EL_TransformationSaveData))
			{
				EL_TransformationSaveData.Cast(componentTuple.m_aSaveData.Get(0)).ApplyTo(worldEntity);
				continue;
			}
			
			array<Managed> outComponents();
			worldEntity.FindComponents(EL_ComponentSaveDataType.Get(componentTuple.m_tType), outComponents);
			
			foreach(EL_ComponentSaveDataBase componentSaveData : componentTuple.m_aSaveData)
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
		
		worldEntity.Update();
		
		return true;
	}
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("dataLayoutVersion", 1);
		saveContext.WriteValue("id", GetId());
		saveContext.WriteValue("prefab", m_Prefab.Substring(1, m_Prefab.IndexOf("}") - 1));
		
		saveContext.StartObject("components");
		saveContext.WriteValue("dataLayoutVersion", 1); // Workaround until https://feedback.bistudio.com/T166982 is fixed
		
		// Include components order info so they can be read back in the same order again (important for binary formats)
		array<string> componentTypesInOrder();
		array<EL_ComponentSaveDataBase> writeComponents();
	
		foreach(EL_TComponentDataTuple componentTuple : m_aComponentsSaveData)
		{
			// If the same component type is included multiple times its written mutiple times
			foreach(EL_ComponentSaveDataBase component : componentTuple.m_aSaveData)
			{
				componentTypesInOrder.Insert(EL_DbName.Get(componentTuple.m_tType));
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
	
		string prefab;
		loadContext.ReadValue("prefab", prefab);
		m_Prefab = string.Format("{%1}", prefab);
	
		loadContext.StartObject("components");
		loadContext.ReadValue("dataLayoutVersion", dataLayoutVersion); // Workaround until https://feedback.bistudio.com/T166982 is fixed
		
		array<string> componentNamesInOrder;
		loadContext.ReadValue("order", componentNamesInOrder);
		
		m_aComponentsSaveData = new array<ref EL_TComponentDataTuple>();
		
		EL_TComponentDataTuple currentTuple;
	
		foreach(int idx, string componentName : componentNamesInOrder)
		{
			typename componentType = EL_DbName.GetTypeByName(componentName);
			EL_ComponentSaveDataBase componentSaveData = EL_ComponentSaveDataBase.Cast(componentType.Spawn());
			loadContext.ReadValue(idx.ToString(), componentSaveData);
		
			if(!currentTuple || currentTuple.m_tType != componentType)
			{
				currentTuple = new EL_TComponentDataTuple(componentType, {componentSaveData});
				m_aComponentsSaveData.Insert(currentTuple);
				continue;
			}
		
			currentTuple.m_aSaveData.Insert(componentSaveData);
		}
		
		loadContext.EndObject();
		
		return true;
	}
}

class EL_TComponentDataTuple
{
	typename m_tType;
	ref array<ref EL_ComponentSaveDataBase> m_aSaveData;
	
	void EL_TComponentDataTuple(typename type, array<ref EL_ComponentSaveDataBase> saveData)
	{
		m_tType = type;
		m_aSaveData = saveData;
	}
}
