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
	
	// Component init
	protected ref Managed m_pSaveDataBuffer;
	
	// Auto save system
	protected float m_fAutoSaveInterval;
	protected float m_fAutoSaveAccumultor;
	protected int m_iAutoSaveIterations;
	
	// Root instance tracking
	protected ref EL_PersistentRootEntityCollection m_pRootEntityCollection;
	protected ref map<EL_PersistentScriptedStateBase, bool> m_mRootScriptedStates;
	
	// Only used during setup
	protected ref EL_PersistentBakedEntityNameIdMapping m_pBakedEntityNameIdMapping;
	protected ref map<string, ref EL_EntitySaveDataBase> m_mInitEntitySaveData;
	protected ref map<string, IEntity> m_mBackedEntities;
	
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
			// TODO: A buffered dbcontext/driver could be added that collects all AddOrUpdate and Remove calls per dbentity type and bulk processes them when it makes sense
			//			- e.g. BufferedJsonWebApiDbDriver
			
			// TODO: Read persistence db source from server config.
			m_pDbContext = EL_DbContextFactory.GetContext();
		}
		
		return m_pDbContext;
	}
	
	void AutoSave()
	{
		m_fAutoSaveAccumultor = 0;
		
		thread AutoSaveThreadImpl();
	}
	
	protected void AutoSaveThreadImpl()
	{
		int nSaveOperation = 0;
		
		foreach(EL_PersistenceComponent persistenceComponent, bool autoSaveEnabled : m_pRootEntityCollection.m_mRootPersistenceComponents)
		{
			if(!autoSaveEnabled) continue;
			
			if((IsActive()) && (nSaveOperation > 0) && (nSaveOperation % m_iAutoSaveIterations == 0))
			{
				Sleep(100); //Wait a few frames
			}
			
			persistenceComponent.Save();
			nSaveOperation++;
		}
		
		foreach(EL_PersistentScriptedStateBase scriptedState, bool autoSaveEnabled : m_mRootScriptedStates)
		{
			if(!autoSaveEnabled) continue;
			
			if((IsActive()) && (nSaveOperation > 0) && (nSaveOperation % m_iAutoSaveIterations == 0))
			{
				Sleep(100); //Wait a few frames
			}
			
			scriptedState.Save();
			nSaveOperation++;
		}
		
		m_pRootEntityCollection.Save(GetDbContext());
	}
	
	protected void ShutDownSave()
	{
		foreach(EL_PersistenceComponent persistenceComponent, auto _ : m_pRootEntityCollection.m_mRootPersistenceComponents)
		{
			EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(persistenceComponent.GetOwner()));
			if(!settings.m_bShutdownsave) continue;
			
			persistenceComponent.Save();
		}
		
		foreach(EL_PersistentScriptedStateBase scriptedState, auto _ : m_mRootScriptedStates)
		{
			if(!EL_PersistentScriptedStateSettings.Get(scriptedState.Type()).m_bShutDownSave) continue;
			
			scriptedState.Save();
		}
		
		m_pRootEntityCollection.Save(GetDbContext());
	}
	
	string GeneratePersistentId()
	{
		return EL_DbEntityIdGenerator.Generate();
	}
	
	protected void PrepareInitalWorldStateThreadImpl()
	{	
		// Remove baked entities that shall no longer be root entities in the world
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
		
		// Remove any removal entries for baked objects that no longer exist
		foreach(string staleId : staleIds)
		{
			int idx = m_pRootEntityCollection.m_aRemovedBackedEntities.Find(staleId);
			if(idx != -1) m_pRootEntityCollection.m_aRemovedBackedEntities.Remove(idx);
		}
		
		// Spawn additional entities from the previously bulk loaded data
		foreach(auto _, set<string> persistentIds : m_pRootEntityCollection.m_mSelfSpawnDynamicEntities)
		{
			foreach(string persistentId : persistentIds)
			{
				SpawnWorldEntity(m_mInitEntitySaveData.Get(persistentId));
			}
		}
		
		// Save any mapping or root entity changes detected during world init
		m_pBakedEntityNameIdMapping.Save(GetDbContext());
		if(staleIds.Count() > 0) m_pRootEntityCollection.Save(GetDbContext());
		
		// Free memory as it not needed after setup
		m_mBackedEntities = null;
		m_mInitEntitySaveData = null;
		m_pBakedEntityNameIdMapping = null;
		
		Print("EL_PersistenceManager::PrepareInitalWorldState() -> Complete.");
	}
	
	IEntity SpawnWorldEntity(EL_EntitySaveDataBase saveData)
	{
		if(!saveData || !saveData.GetId()) return null;
		
		IEntity worldEntity;
		
		if(m_mBackedEntities)
		{
			// Try baked enties during world setup.
			worldEntity = m_mBackedEntities.Get(saveData.GetId());
		}
		
		if(!worldEntity)
		{
			Resource resource = Resource.Load(saveData.m_Prefab);
			if(!resource.IsValid())
			{
				Debug.Error(string.Format("Invalid prefab type '%1' on '%2:%3' could not be spawned. Ignored.", saveData.m_Prefab, saveData.Type(), saveData.GetId()));
				return null;
			}
			
			m_pSaveDataBuffer = saveData;
			worldEntity = GetGame().SpawnEntityPrefab(resource);
			m_pSaveDataBuffer = null;
			
			if(!worldEntity)
			{
				Debug.Error(string.Format("Failed to spawn entity '%1:%2'. Ignored.", saveData.Type(), saveData.GetId()));
				return null;
			}
		}
		
		return worldEntity;
	} 
	
	EL_PersistentScriptedStateBase SpawnScriptedState(EL_ScriptedStateSaveDataBase saveData)
	{
		if(!saveData || !saveData.GetId()) return null;

		typename scriptedStateType = EL_PersistentScriptedStateSettings.GetScriptedStateType(saveData.Type());
		
		m_pSaveDataBuffer = saveData;
		EL_PersistentScriptedStateBase state = EL_PersistentScriptedStateBase.Cast(scriptedStateType.Spawn());
		m_pSaveDataBuffer = null;
		
		if(!state || !state.GetPersistentId()) // Must have valid persistence id upon creation or something went terribly wrong
		{
			Debug.Error(string.Format("Failed to spawn scripted state '%1:%2'. Ignored.", saveData.Type(), saveData.GetId()));
			return null;
		}
		
		return state;
	}
	
	protected void LoadSetupData()
	{
		m_pBakedEntityNameIdMapping = EL_DbEntityHelper<EL_PersistentBakedEntityNameIdMapping>.GetRepository(GetDbContext()).FindSingleton().GetEntity();
		m_pRootEntityCollection = EL_DbEntityHelper<EL_PersistentRootEntityCollection>.GetRepository(GetDbContext()).FindSingleton().GetEntity();
		
		// Collect type and ids of inital world entities for bulk load
		map<typename, ref set<string>> bulkLoad();
		foreach(typename saveType, set<string> persistentIds: m_pRootEntityCollection.m_mSelfSpawnDynamicEntities)
		{
			set<string> loadIds();
			loadIds.Copy(persistentIds);
			bulkLoad.Set(saveType, loadIds);
		}
		foreach(string name, Tuple2<string, typename> idTypeTuple : m_pBakedEntityNameIdMapping.m_mNameIdMapping)
		{
			if(m_pRootEntityCollection.m_aRemovedBackedEntities.Contains(idTypeTuple.param1))
			{
				continue; //Skip baked objects that will be deleted in load phase
			}
			
			set<string> loadIds = bulkLoad.Get(idTypeTuple.param2);
			
			if(!loadIds)
			{
				loadIds = new set<string>();
				bulkLoad.Set(idTypeTuple.param2, loadIds);
			}
			
			loadIds.Insert(idTypeTuple.param1);
		}
		
		// Load all known inital entity types from db, both baked and dynamic in one bulk operation
		foreach(typename saveDataType, set<string> persistentIds : bulkLoad)
		{
			array<string> loadIds();
			loadIds.Resize(persistentIds.Count());
			foreach(int idx, string id : persistentIds)
			{
				loadIds.Set(idx, id);
			}
			
			array<ref EL_DbEntity> findResults = GetDbContext().FindAll(saveDataType, EL_DbFind.Id().EqualsAnyOf(loadIds)).GetEntities();
			if(!findResults) continue;
			
			foreach(EL_DbEntity findResult : findResults)
			{
				EL_EntitySaveDataBase saveData = EL_EntitySaveDataBase.Cast(findResult);
				if(!saveData)
				{
					Debug.Error(string.Format("Unexpected database find result type '%1' encountered during entity load. Ignored.", findResult.Type()));
					continue;
				}
				
				m_mInitEntitySaveData.Set(saveData.GetId(), saveData);
			}
		}
	}
	
	static EL_PersistenceManager GetInstance()
	{
		// Persistence logic only runs on the server machine
		if(!IsPersistenceMaster()) return null;
		
		if(!s_pInstance)
		{
			s_pInstance = new EL_PersistenceManagerInternal();
			
			//Reset the singleton when a new mission is loaded to free all memory and have a clean startup again.
			GetGame().m_OnMissionSetInvoker.Insert(Reset);
		}
		
		return s_pInstance;
	}
	
	protected void EL_PersistenceManager()
	{
		m_eState = EL_EPersistenceManagerState.WORLD_INIT;
		
		m_mRootScriptedStates = new map<EL_PersistentScriptedStateBase, bool>();
		
		m_mInitEntitySaveData = new map<string, ref EL_EntitySaveDataBase>();
		m_mBackedEntities = new map<string, IEntity>();
		
		LoadSetupData();
	}
	
	protected static void Reset()
	{
		s_pInstance = null;
	}
}

class EL_PersistenceManagerInternal : EL_PersistenceManager
{
	static EL_PersistenceManagerInternal GetInternalInstance()
	{
		return EL_PersistenceManagerInternal.Cast(GetInstance());
	}

	void RegisterSaveRoot(notnull EL_PersistenceComponent persistenceComponent, bool baked, typename selfSpawnType, bool autoSave)
	{
		m_pRootEntityCollection.Add(persistenceComponent, baked, selfSpawnType, autoSave, m_eState);
	}
	
	void RegisterSaveRoot(notnull EL_PersistentScriptedStateBase scripedState, bool autoSave)
	{
		m_mRootScriptedStates.Set(scripedState, autoSave);
	}
	
	void UnregisterSaveRoot(notnull EL_PersistenceComponent persistenceComponent, bool baked, typename selfSpawnType)
	{
		m_pRootEntityCollection.Remove(persistenceComponent, baked, selfSpawnType);
	}
	
	void UnregisterSaveRoot(notnull EL_PersistentScriptedStateBase scripedState)
	{
		m_mRootScriptedStates.Remove(scripedState);
	}
	
	string GetPersistentId(notnull IEntity worldEntity)
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
				id = GeneratePersistentId();
                EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
                EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(worldEntity));
                m_pBakedEntityNameIdMapping.Insert(name, id, settings.m_tSaveDataTypename);
			}
			
			m_mBackedEntities.Set(id, worldEntity);
		}
		else
		{
			id = GeneratePersistentId();
		}
		
		return id;
	}
	
	void SetSaveDataBuffer(Managed saveData)
	{
		m_pSaveDataBuffer = saveData;
	}
	
	EL_EntitySaveDataBase GetEntitySaveDataBuffer(string persistentId)
	{
		EL_EntitySaveDataBase result;
		
		// Get existing pre loaded data for baked objects 
		if(m_eState == EL_EPersistenceManagerState.WORLD_INIT)
		{
			result = m_mInitEntitySaveData.Get(persistentId);
		}
		else
		{
			result = EL_EntitySaveDataBase.Cast(m_pSaveDataBuffer);
		}
		
		return result;
	}
	
	EL_ScriptedStateSaveDataBase GetScriptedStateSaveDataBuffer()
	{
		return EL_ScriptedStateSaveDataBase.Cast(m_pSaveDataBuffer);
	}
	
	event void OnPostInit(IEntity gameMode)
	{
		EL_PersistenceManagerComponent managerComponent = EL_PersistenceManagerComponent.Cast(gameMode.FindComponent(EL_PersistenceManagerComponent));
		EL_PersistenceManagerComponentClass settings = EL_PersistenceManagerComponentClass.Cast(managerComponent.GetComponentData(gameMode));
		
		if(settings.m_bEnabled)
		{
			m_fAutoSaveInterval = settings.m_fInterval;
			m_iAutoSaveIterations = Math.Clamp(settings.m_iIterations, 1, 128);
		}
	}
	
	event void OnPostFrame(float timeSlice)
	{
		// Handle auto-save
		m_fAutoSaveAccumultor += timeSlice;
		
		if((m_fAutoSaveInterval > 0) && (m_fAutoSaveAccumultor >= m_fAutoSaveInterval))
		{
			AutoSave();
		}
	}
	
	event void OnWorldPostProcess(World world)
	{
		m_eState = EL_EPersistenceManagerState.ACTIVE;

		thread PrepareInitalWorldStateThreadImpl();
	}
	
	event void OnGameEnd()
	{
		m_eState = EL_EPersistenceManagerState.SHUTDOWN;
		
		// Call without thread to be blocking on shutdown
		ShutDownSave();
		
		//Cleanup after end of current session
		Reset(); // TODO: Test game end vs game mode end. which one is guaranteed to trigger before any world entity dtors
	}
}
