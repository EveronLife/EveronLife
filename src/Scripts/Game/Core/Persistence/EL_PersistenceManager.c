enum EL_EPersistenceManagerState
{
	WORLD_INIT,
	ACTIVE,
	SHUTDOWN
}

class EL_PersistenceManager
{
	protected static ref EL_PersistenceManager s_pInstance;
	
	protected EL_EPersistenceManagerState m_eState;
	protected EL_DbContext m_pDbContext;
	
	// Auto save system
	protected ref set<EL_PersistenceComponent> m_aAutoSaveComponents;
	protected ref set<EL_PersistentScriptedStateBase> m_aAutoSaveScriptedState;
	protected float m_fAutoSaveInterval;
	protected float m_fAutoSaveAccumultor;
	protected int m_iAutoSaveIterations;
	
	// Root entity tracking
	protected ref EL_PersistentRootEntityCollection m_pRootEntityCollection;
	protected bool m_bRootEntityFlushRequested;
	
	// Only used during setup
	protected ref EL_PersistentBakedEntityNameIdMapping m_pBakedEntityNameIdMapping;
	protected ref map<string, IEntity> m_mBackedEntities;
	protected string m_sPersistentIdBuffer;
	
	static bool IsPersistenceMaster()
	{
		return GetGame().InPlayMode() && Replication.IsServer();
	}
	
	bool IsActive()
	{
		return GetGame().InPlayMode() && m_eState == EL_EPersistenceManagerState.ACTIVE;
	}
	
	EL_EPersistenceManagerState GetState()
	{
		return m_eState;
	}
	
	EL_DbContext GetDbContext()
	{
		if(!m_pDbContext)
		{
			// TODO: Read persistence db source from server config.
			m_pDbContext = EL_DbContextFactory.GetContext();
		}

		return m_pDbContext;
	}
	
	protected void SubscribeAutoSave(notnull IEntity worldEntity)
	{
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		if(!persistenceComponent) return;
		
		m_aAutoSaveComponents.Insert(persistenceComponent);
	}
	
	protected void SubscribeAutoSave(notnull EL_PersistentScriptedStateBase scripedState)
	{
		m_aAutoSaveScriptedState.Insert(scripedState);
	}
	
	protected void UnsubscribeAutoSave(notnull IEntity worldEntity)
	{
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		if(!persistenceComponent) return;
		
		int idx = m_aAutoSaveComponents.Find(persistenceComponent);
		if(idx != -1) m_aAutoSaveComponents.Remove(idx);
	}
	
	protected void UnsubscribeAutoSave(notnull EL_PersistentScriptedStateBase scripedState)
	{
		int idx = m_aAutoSaveScriptedState.Find(scripedState);
		if(idx != -1) m_aAutoSaveScriptedState.Remove(idx);
	}
	
	void AutoSave()
	{
		m_fAutoSaveAccumultor = 0;
		
		thread AutoSaveThreadImpl();
	}
	
	protected void AutoSaveThreadImpl()
	{
		int nSaveOperation = 0;
		
		foreach(EL_PersistenceComponent persistenceComponent : m_aAutoSaveComponents)
		{
			if((IsActive()) && (nSaveOperation > 0) && (nSaveOperation % m_iAutoSaveIterations == 0))
			{
				Sleep(100); //Wait a few frames
			}
			
			persistenceComponent.Save();
			nSaveOperation++;
		}
		
		foreach(EL_PersistentScriptedStateBase scriptedState : m_aAutoSaveScriptedState)
		{
			if((IsActive()) && (nSaveOperation > 0) && (nSaveOperation % m_iAutoSaveIterations == 0))
			{
				Sleep(100); //Wait a few frames
			}
			
			scriptedState.Save();
			nSaveOperation++;
		}
	}
	
	protected void RegisterRootEntity(typename saveDataType, string persistentId, bool baked, bool flush = false)
	{
		if(baked)
		{
			int idx = m_pRootEntityCollection.m_aRemovedBackedEntities.Find(persistentId);
			if(idx != -1) m_pRootEntityCollection.m_aRemovedBackedEntities.Remove(idx);
		}
		else
		{
			set<string> ids = m_pRootEntityCollection.m_mAddtionalDynamicEntities.Get(saveDataType);
			
			if(!ids)
			{
				ids = new set<string>();
				m_pRootEntityCollection.m_mAddtionalDynamicEntities.Set(saveDataType, ids);
			}
			
			ids.Insert(persistentId);
		}
		
		if(flush) m_bRootEntityFlushRequested = true;
	}

	protected void UnregisterRootEntity(typename saveDataType, string persistentId, bool baked, bool flush = false)
	{
		if(baked)
		{
			m_pRootEntityCollection.m_aRemovedBackedEntities.Insert(persistentId);
		}
		else
		{
			set<string> ids = m_pRootEntityCollection.m_mAddtionalDynamicEntities.Get(saveDataType);
		
			if(ids)
			{
				int idx = ids.Find(persistentId);
				if(idx != -1) ids.Remove(idx);
			}
		}
		
		if(flush) m_bRootEntityFlushRequested = true;
	}
		
	protected void FlushRootEntityRegistrationsThreadImpl()
	{
		m_bRootEntityFlushRequested = false;
		
		// Remove collection if it only holds default values
		if (m_pRootEntityCollection.m_aRemovedBackedEntities.Count() == 0 && 
			m_pRootEntityCollection.m_mAddtionalDynamicEntities.Count() == 0)
		{
			// Only need to call db if it was previously saved (aka it has an id)
			if(m_pRootEntityCollection.HasId()) GetDbContext().RemoveAsync(m_pRootEntityCollection);
		}
		else
		{
			GetDbContext().AddOrUpdateAsync(m_pRootEntityCollection);
		}
	}
	
	protected string GeneratePersistentId(notnull IEntity worldEntity)
	{
		string id;

		// Load if from baked object name->id mapping
		if(m_eState == EL_EPersistenceManagerState.WORLD_INIT)
		{
			string name = worldEntity.GetName();
			if(!name)
			{
				Debug.Error(string.Format("Baked world entity '%1'@%2 needs to have a name to be trackable by the persistence system.", 
					EL_Utils.GetPrefabName(worldEntity), 
					worldEntity.GetOrigin()));
				
				return string.Empty;
			}
			
			id = m_pBakedEntityNameIdMapping.GetIdByName(name);
			
			// Name was not yet mapped so generate and id for it and add it to the mapping
			if(!id)
			{
				id = EL_DbEntityIdGenerator.Generate();
				
				EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
				if(!persistenceComponent) string.Empty;
				EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(worldEntity));
				m_pBakedEntityNameIdMapping.Insert(name, id, settings.m_tSaveDataTypename);
			}
			
			m_mBackedEntities.Set(id, worldEntity);
		}
		else if(m_sPersistentIdBuffer)
		{
			id = m_sPersistentIdBuffer;
			m_sPersistentIdBuffer = string.Empty;
		}
		else
		{
			id = EL_DbEntityIdGenerator.Generate();
		}
		
		Print(string.Format("EL_PersistenceManager: Entity '%1'@%2 was assigned persistent id '%3'.", EL_Utils.GetPrefabName(worldEntity), worldEntity.GetOrigin(), id));
		
		return id;
	}

	protected string GeneratePersistentId(notnull EL_PersistentScriptedStateBase scripedState)
	{
		string id;
		
		if(m_sPersistentIdBuffer)
		{
			id = m_sPersistentIdBuffer;
			m_sPersistentIdBuffer = string.Empty;
		}
		else
		{
			id = EL_DbEntityIdGenerator.Generate();
		}
		
		Print(string.Format("EL_PersistenceManager: Scripted state '%1' was assigned persistent id '%2'.", scripedState, id));
		
		return id;
	}
	
	event void OnWorldPostProcessThreadImpl(World world)
	{
		m_eState = EL_EPersistenceManagerState.ACTIVE;
		
		UpdateBakedEntityNameIdMapping();
		
		PrepareInitalWorldState();
	}
	
	protected void UpdateBakedEntityNameIdMapping()
	{
		// Save any changes to the mapping
		m_pBakedEntityNameIdMapping.Save(GetDbContext());

		// Free the mapping as it no longer needed
		m_pBakedEntityNameIdMapping = null;
	}
	
	protected void PrepareInitalWorldState()
	{	
		// 1. Remove baked entities that shall no longer be root entities in the world
		array<string> staleIds();
		foreach(string persistentId : m_pRootEntityCollection.m_aRemovedBackedEntities)
		{
			IEntity worldEntity = m_mBackedEntities.Get(persistentId);
			if(!worldEntity)
			{
				staleIds.Insert(persistentId);
				continue;
			}
			
			Print(string.Format("EL_PersistenceManager::PrepareInitalWorldState() -> Deleting baked entity '%1'@%2.", EL_Utils.GetPrefabName(worldEntity), worldEntity.GetOrigin()), LogLevel.SPAM);
			SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
		}
		
		// 1.1. Remove any removal entries for baked objects that no longer exist
		foreach(string staleId : staleIds)
		{
			int idx = m_pRootEntityCollection.m_aRemovedBackedEntities.Find(staleId);
			if(idx != -1) m_pRootEntityCollection.m_aRemovedBackedEntities.Remove(idx);
		}
		if(staleIds.Count() > 0)
		{
			m_bRootEntityFlushRequested = true;
			Print("EL_PersistenceManager::PrepareInitalWorldState() -> Deleting stale baked entity removal ids.");
		}
		
		// 2. Load all known root entity types from db, both baked and dynamic in one bulk operation
		map<typename, ref set<string>> bulkLoad();
		foreach(typename saveType, set<string> persistentIds: m_pRootEntityCollection.m_mAddtionalDynamicEntities)
		{
			set<string> loadIds();
			loadIds.Copy(persistentIds);
			bulkLoad.Set(saveType, loadIds);
		}
		foreach(string persistentId, IEntity bakedEntity : m_mBackedEntities)
		{
			if(!persistentId || !bakedEntity) continue;
			
			EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(bakedEntity.FindComponent(EL_PersistenceComponent));
			EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(bakedEntity));
			
			set<string> loadIds = bulkLoad.Get(settings.m_tSaveDataTypename);
			
			if(!loadIds)
			{
				loadIds = new set<string>();
				bulkLoad.Set(settings.m_tSaveDataTypename, loadIds);
			}
			
			loadIds.Insert(persistentId);
		}
		
		foreach(typename saveDataType, set<string> persistentIds : bulkLoad)
		{
			array<string> loadIds();
			loadIds.Resize(persistentIds.Count());
			foreach(int idx, string id : persistentIds)
			{
				loadIds.Set(idx, id);
			}
			
			// 2.1. Apply save data to existing world entity or spawn a dynamic one to apply to
			array<ref EL_DbEntity> findResults = GetDbContext().FindAll(saveDataType, EL_DbFind.Id().EqualsAnyOf(loadIds)).GetEntities();
			if(!findResults) continue;
			
			//Print(string.Format("EL_PersistenceManager::LoadWorldEntities() -> Found %1 entities of type '%2'.", findResults.Count(), saveDataType));
			
			foreach(EL_DbEntity findResult : findResults)
			{
				//Print(string.Format("EL_PersistenceManager::LoadWorldEntities() -> Loading entity '%1:%2'.", saveDataType,findResult.GetId()));
				
				EL_EntitySaveDataBase saveData = EL_EntitySaveDataBase.Cast(findResult);
				if(!saveData)
				{
					Debug.Error(string.Format("Unexpected database find result type '%1' encountered during entity load. Ignored.", findResult.Type()));
					continue;
				}
				
				SpawnWorldEntity(saveData);
			}
		}
		
		// 3. Free memory as it not needed after setup
		m_mBackedEntities = null;
		
		Print("EL_PersistenceManager::PrepareInitalWorldState() -> Complete.");
	}

	IEntity SpawnWorldEntity(EL_EntitySaveDataBase saveData)
	{
		if(!saveData) return null;
		
		IEntity worldEntity = m_mBackedEntities.Get(saveData.GetId());
		if(!worldEntity)
		{
			Resource resource = Resource.Load(saveData.m_Prefab);
			if(!resource.IsValid())
			{
				Debug.Error(string.Format("Invalid prefab type '%1' on '%2:%3' could not be spawned. Ignored.", saveData.m_Prefab, saveData.Type(), saveData.GetId()));
				return null;
			}
			
			m_sPersistentIdBuffer = saveData.GetId();
			worldEntity = GetGame().SpawnEntityPrefab(resource);
			if(!worldEntity)
			{
				Debug.Error(string.Format("Failed to spawn entity '%1:%2'. Ignored.", saveData.Type(), saveData.GetId()));
				return null;
			}
		}
		
		if(!saveData.ApplyTo(worldEntity))
		{
			Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity. Ignored.", saveData.Type(), saveData.GetId()));
			SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
			return null;
		}
		
		return worldEntity;
	} 
	
	EL_PersistentScriptedStateBase SpawnScriptedState(EL_ScriptedStateSaveDataBase saveData)
	{
		if(!saveData) return null;
		
		m_sPersistentIdBuffer = saveData.GetId();
		
		typename scriptedStateType = EL_ScriptedStateSaveDataType.GetScriptedStateType(saveData.Type());
		EL_PersistentScriptedStateBase state = EL_PersistentScriptedStateBase.Cast(scriptedStateType.Spawn());
		if(!state)
		{
			Debug.Error(string.Format("Failed to spawn scripted state '%1:%2'. Ignored.", saveData.Type(), saveData.GetId()));
			return null;
		}
		
		if(!saveData.ApplyTo(state))
		{
			Debug.Error(string.Format("Failed apply save-data '%1:%2' to scripted state. Ignored.", saveData.Type(), saveData.GetId()));
			return null;
		}
		
		return state;
	}
	
	protected event void OnGameEnd()
	{
		m_eState = EL_EPersistenceManagerState.SHUTDOWN;
		
		// Call without thread to be blocking on shutdown
		AutoSaveThreadImpl();
		
		if(m_bRootEntityFlushRequested)
		{
			FlushRootEntityRegistrationsThreadImpl();
		}
		
		//Cleanup after end of current session
		Reset(); // TODO: Test if this event is called when existing server and returning to main menu (assuming player hosted instance)
	}
	
	protected event void OnPostFrame(float timeSlice)
	{
		m_fAutoSaveAccumultor += timeSlice;
		
		if((m_fAutoSaveInterval > 0) && (m_fAutoSaveAccumultor >= m_fAutoSaveInterval))
		{
			AutoSave();
		}
		
		if(m_bRootEntityFlushRequested)
		{
			thread FlushRootEntityRegistrationsThreadImpl();
		}
	}
	
	protected event void OnPostInit(IEntity gameMode)
	{
		EL_PersistenceManagerComponent managerComponent = EL_PersistenceManagerComponent.Cast(gameMode.FindComponent(EL_PersistenceManagerComponent));
		EL_PersistenceManagerComponentClass settings = EL_PersistenceManagerComponentClass.Cast(managerComponent.GetComponentData(gameMode));
		
		if(settings.m_bEnabled)
		{
			m_fAutoSaveInterval = settings.m_fInterval;
			m_iAutoSaveIterations = Math.Clamp(settings.m_iIterations, 1, 128);
		}
	}
	
	protected void LoadBakedEntityNameIdMapping()
	{
		m_pBakedEntityNameIdMapping = EL_DbEntityHelper<EL_PersistentBakedEntityNameIdMapping>.GetRepository(GetDbContext()).FindFirst().GetEntity();
		
		if(!m_pBakedEntityNameIdMapping)
		{
			m_pBakedEntityNameIdMapping = new EL_PersistentBakedEntityNameIdMapping();
		}
	}
	
	protected void LoadRootEntityCollection()
	{
		m_pRootEntityCollection = EL_DbEntityHelper<EL_PersistentRootEntityCollection>.GetRepository(GetDbContext()).FindFirst().GetEntity();
		
		if(!m_pRootEntityCollection)
		{
			m_pRootEntityCollection = new EL_PersistentRootEntityCollection();
		}
	}
	
	static EL_PersistenceManager GetInstance()
	{
		// Persistence logic only runs on the server machine
		if(!IsPersistenceMaster()) return null;
		
		if(!s_pInstance)
		{
			s_pInstance = new EL_PersistenceManager();
			
			//Reset the singleton when a new mission is loaded to free all memory and have a clean startup again.
			GetGame().m_OnMissionSetInvoker.Insert(Reset);
		}
		
		return s_pInstance;
	}
	
	protected void EL_PersistenceManager()
	{
		m_eState = EL_EPersistenceManagerState.WORLD_INIT;
		m_aAutoSaveComponents = new set<EL_PersistenceComponent>();
		m_aAutoSaveScriptedState = new set<EL_PersistentScriptedStateBase>();
		m_mBackedEntities = new map<string, IEntity>();

		LoadBakedEntityNameIdMapping();
		LoadRootEntityCollection();
	}
	
	protected static void Reset()
	{
		s_pInstance = null;
	}
}
