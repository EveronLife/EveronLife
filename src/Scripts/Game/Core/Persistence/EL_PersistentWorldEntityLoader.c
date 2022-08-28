class EL_PersistentWorldEntityLoader
{
	protected static ref map<string, typename> m_SaveDataTypeCache;
	
	protected static typename GetSaveDataType(string prefab)
	{
		if (!m_SaveDataTypeCache) m_SaveDataTypeCache = new map<string, typename>();
		
		typename resultType = m_SaveDataTypeCache.Get(prefab);
		
		if (!resultType)
		{
			Resource resource = Resource.Load(prefab);
			if (resource && resource.IsValid())
			{
				IEntitySource entitySource = resource.GetResource().ToEntitySource();
				for (int nComponentSource = 0, count = entitySource.GetComponentCount(); nComponentSource < count; nComponentSource++)
				{
					IEntityComponentSource componentSource = entitySource.GetComponent(nComponentSource);
					typename componentType = componentSource.GetClassName().ToType();
					if (componentType.IsInherited(EL_PersistenceComponent))
					{
						BaseContainer saveDataContainer = componentSource.GetObject("m_pSaveData");
						if (saveDataContainer)
						{
							resultType = saveDataContainer.GetClassName().ToType();
							break;
						}
					}
				}
			}
		}
		
		m_SaveDataTypeCache.Set(prefab, resultType);
		
		return resultType;
	}
	
	static IEntity Load(string prefab, string persistentId)
	{
		typename type = GetSaveDataType(prefab); // Can not be inlined because of typename crash bug
		return Load(type, persistentId); 
	}
		
	static IEntity Load(typename saveDataType, string persistentId)
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
	    array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, EL_DbFind.Id().Equals(persistentId), limit: 1).GetEntities();
	    if(!findResults || findResults.Count() != 1) return null;
	    return persistenceManager.SpawnWorldEntity(EL_EntitySaveDataBase.Cast(findResults.Get(0)));
	}
	
	static void LoadAsync(typename saveDataType, string persistentId, notnull EL_WorldEntityLoaderCallbackSingle callback)
	{
		EL_WorldEntityLoaderProcessorCallbackSingle processorCallback();
		processorCallback.Setup(callback);
	    EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, EL_DbFind.Id().Equals(persistentId), limit: 1, callback: processorCallback);
	}
	
	static void LoadAsync(string prefab, string persistentId, notnull EL_WorldEntityLoaderCallbackSingle callback)
	{
		typename type = GetSaveDataType(prefab); // Can not be inlined because of typename crash bug
		LoadAsync(type, persistentId, callback);
	}
	
	static array<IEntity> Load(typename saveDataType, array<string> persistentIds)
	{
		array<IEntity> resultWorldEntities();
	    
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
	    array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, EL_DbFind.Id().EqualsAnyOf(persistentIds)).GetEntities();
	    if (findResults)
		{
		    foreach(EL_DbEntity findResult : findResults)
		    {
		        IEntity worldEntity = persistenceManager.SpawnWorldEntity(EL_EntitySaveDataBase.Cast(findResult));
		        if (worldEntity) resultWorldEntities.Insert(worldEntity);			
		    }
		}
	    
	    return resultWorldEntities;
	}
	
	static array<IEntity> Load(string prefab, array<string> persistentIds)
	{
		typename type = GetSaveDataType(prefab); // Can not be inlined because of typename crash bug
		return Load(type, persistentIds);
	}
	
	static void LoadAsync(typename saveDataType, array<string> persistentIds, notnull EL_WorldEntityLoaderCallbackMultiple callback)
	{
		EL_WorldEntityLoaderProcessorCallbackMultiple processorCallback();
		processorCallback.Setup(callback);
		EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, EL_DbFind.Id().EqualsAnyOf(persistentIds), callback: processorCallback);
	}
	
	static void LoadAsync(string prefab, array<string> persistentIds, notnull EL_WorldEntityLoaderCallbackMultiple callback)
	{
		typename type = GetSaveDataType(prefab); // Can not be inlined because of typename crash bug
		LoadAsync(type, persistentIds, callback);
	}
}

class EL_WorldEntityLoaderCallback : EL_Callback
{
}

class EL_WorldEntityLoaderCallbackSingle : EL_WorldEntityLoaderCallback
{
    void Invoke(IEntity data)
    {
        if (m_pInvokeInstance && 
            m_sInvokeMethodName &&
            GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, data)) return;
        
        OnComplete(m_pContext, data);
    }
    
    void OnComplete(Managed context, IEntity data);
}

class EL_WorldEntityLoaderCallbackMultiple : EL_WorldEntityLoaderCallback
{
    void Invoke(array<IEntity> data)
    {
        if (m_pInvokeInstance && 
            m_sInvokeMethodName &&
            GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, data)) return;
        
        OnComplete(m_pContext, data);
    }
    
    void OnComplete(Managed context, array<IEntity> data);
}

class EL_WorldEntityLoaderProcessorCallbackSingle : EL_DbFindCallbackSingle<EL_EntitySaveDataBase>
{
	ref EL_WorldEntityLoaderCallbackSingle m_pOuterCallback;
	
	override void OnSuccess(Managed context, EL_EntitySaveDataBase resultData)
	{
		IEntity resultWorldEntity;
		if (resultData) resultWorldEntity = EL_PersistenceManager.GetInstance().SpawnWorldEntity(resultData);
		m_pOuterCallback.Invoke(resultWorldEntity);
	}
	
	override void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode)
	{
		m_pOuterCallback.Invoke(null);
	}
	
	void Setup(EL_WorldEntityLoaderCallbackSingle outerCallback)
	{
		m_pOuterCallback = outerCallback;
	}
}

class EL_WorldEntityLoaderProcessorCallbackMultiple : EL_DbFindCallback<EL_EntitySaveDataBase>
{
	ref EL_WorldEntityLoaderCallbackMultiple m_pOuterCallback;
	
	override void OnSuccess(Managed context, array<ref EL_EntitySaveDataBase> resultData)
	{
		array<IEntity> resultWorldEntities();
		
		if (resultData)
		{
			EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
			
			foreach (EL_EntitySaveDataBase saveData : resultData)
			{
				IEntity worldEntity = persistenceManager.SpawnWorldEntity(saveData);
				if (worldEntity) resultWorldEntities.Insert(worldEntity);
			}
		}
		
		m_pOuterCallback.Invoke(resultWorldEntities);
	}
	
	override void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode)
	{
		m_pOuterCallback.Invoke(new array<IEntity>());
	}
	
	void Setup(EL_WorldEntityLoaderCallbackMultiple outerCallback)
	{
		m_pOuterCallback = outerCallback;
	}
}
