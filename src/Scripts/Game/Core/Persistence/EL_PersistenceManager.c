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
	protected float m_fAutoSaveInterval;
	protected float m_fAutoSaveAccumultor;
	protected int m_iAutoSaveIterations;
	
	// Component init
	protected string m_sNextPersistentId;
	
	// Root instance tracking
	protected ref EL_PersistentRootEntityCollection m_pRootEntityCollection;
	protected ref map<EL_PersistenceComponent, ref Tuple2<bool, bool>> m_mRootPersistenceComponents;
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
		
		foreach(EL_PersistenceComponent persistenceComponent, Tuple2<bool, bool> infoTuple : m_mRootPersistenceComponents)
		{
			if(!infoTuple.param1) continue; //Ignore if no auto-save
			
			UpdateRootEntityCollection(persistenceComponent, infoTuple);
			
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
		foreach(EL_PersistenceComponent persistenceComponent, Tuple2<bool, bool> infoTuple : m_mRootPersistenceComponents)
		{
			EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(persistenceComponent.GetOwner()));
			if(!settings.m_bShutdownsave) continue;
			
			UpdateRootEntityCollection(persistenceComponent, infoTuple);
			
			persistenceComponent.Save();
		}
		
		foreach(EL_PersistentScriptedStateBase scriptedState, auto _ : m_mRootScriptedStates)
		{
			if(!EL_PersistentScriptedStateSettings.Get(scriptedState.Type()).m_bShutDownSave) continue;
			
			scriptedState.Save();
		}
		
		m_pRootEntityCollection.Save(GetDbContext());
	}
	
	protected void UpdateRootEntityCollection(EL_PersistenceComponent persistenceComponent, Tuple2<bool, bool> infoTuple)
	{
		// Double check all root entites as some of them might have been stored or unstored without a manager noticing. Remove once ground item manipulation events are exposed.
		InventoryItemComponent inventoryItemComponent = InventoryItemComponent.Cast(persistenceComponent.GetOwner().FindComponent(InventoryItemComponent));
		if(!inventoryItemComponent) return;
		
		if(!inventoryItemComponent.GetParentSlot())
		{
			m_pRootEntityCollection.Add(persistenceComponent, infoTuple.param2);
		}
		else
		{
			m_pRootEntityCollection.Remove(persistenceComponent, infoTuple.param2);
		}
	}
	
	protected void PrepareInitalWorldStateThreadImpl()
	{	
		Sleep(0); // Push to next frame to not disturb world hash calculation
		
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
			
			m_mBackedEntities.Remove(persistentId);
			Print(string.Format("EL_PersistenceManager::PrepareInitalWorldState() -> Deleting baked entity '%1'@%2.", EL_Utils.GetPrefabName(worldEntity), worldEntity.GetOrigin()), LogLevel.SPAM);
			SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
		}
		
		// Remove any removal entries for baked objects that no longer exist
		foreach(string staleId : staleIds)
		{
			int idx = m_pRootEntityCollection.m_aRemovedBackedEntities.Find(staleId);
			if(idx != -1) m_pRootEntityCollection.m_aRemovedBackedEntities.Remove(idx);
		}
		
		// Apply save data to baked entities
		foreach(string persistentId, IEntity bakedEntity : m_mBackedEntities)
		{
			EL_EntitySaveDataBase saveData = m_mInitEntitySaveData.Get(persistentId);
			if(!saveData) continue;
			
			EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(bakedEntity.FindComponent(EL_PersistenceComponent));
			if(persistenceComponent) persistenceComponent.Load(saveData);
		}
		
		// Spawn additional entities from the previously bulk loaded data
		foreach(auto _, set<string> persistentIds : m_pRootEntityCollection.m_mSelfSpawnDynamicEntities)
		{
			foreach(string persistentId : persistentIds)
			{
				EL_EntitySaveDataBase saveData = m_mInitEntitySaveData.Get(persistentId);
				if(saveData) SpawnWorldEntity(saveData);
			}
		}
		
		// Save any mapping or root entity changes detected during world init
		m_pBakedEntityNameIdMapping.Save(GetDbContext());
		if(staleIds.Count() > 0) m_pRootEntityCollection.Save(GetDbContext());
		
		// Free memory as it not needed after setup
		m_mInitEntitySaveData = null;
		m_pBakedEntityNameIdMapping = null;
		
		Print("EL_PersistenceManager::PrepareInitalWorldState() -> Complete.");
	}
	
	IEntity SpawnWorldEntity(notnull EL_EntitySaveDataBase saveData)
	{
		if(!saveData.GetId()) return null;
		
		Resource resource = Resource.Load(saveData.m_rPrefab);
		if(!resource.IsValid())
		{
			Debug.Error(string.Format("Invalid prefab type '%1' on '%2:%3' could not be spawned. Ignored.", saveData.m_rPrefab, saveData.Type().ToString(), saveData.GetId()));
			return null;
		}
		
		m_sNextPersistentId = saveData.GetId();
		IEntity worldEntity = GetGame().SpawnEntityPrefab(resource);
		m_sNextPersistentId = string.Empty;
		if(!worldEntity)
		{
			Debug.Error(string.Format("Failed to spawn entity '%1:%2'. Ignored.", saveData.Type().ToString(), saveData.GetId()));
			return null;
		}
		
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		if(!persistenceComponent || !persistenceComponent.Load(saveData))
		{
			SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
			return null;
		}
		
		return worldEntity;
	} 
	
	EL_PersistentScriptedStateBase SpawnScriptedState(notnull EL_ScriptedStateSaveDataBase saveData)
	{
		if(!saveData.GetId()) return null;

		typename scriptedStateType = EL_PersistentScriptedStateSettings.GetScriptedStateType(saveData.Type());
		m_sNextPersistentId = saveData.GetId();
		EL_PersistentScriptedStateBase state = EL_PersistentScriptedStateBase.Cast(scriptedStateType.Spawn());
		m_sNextPersistentId = string.Empty;
		if(!state)
		{
			Debug.Error(string.Format("Failed to spawn scripted state '%1:%2'. Ignored.", saveData.Type().ToString(), saveData.GetId()));
			return null;
		}
		
		if(!state.Load(saveData)) return null;
		
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
			//Skip baked objects that will be deleted in load phase
			if(m_pRootEntityCollection.m_aRemovedBackedEntities.Contains(idTypeTuple.param1)) continue; 
			
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
					Debug.Error(string.Format("Unexpected database find result type '%1' encountered during entity load. Ignored.", findResult.Type().ToString()));
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
		
		m_mRootPersistenceComponents = new map<EL_PersistenceComponent, ref Tuple2<bool, bool>>();
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

	void RegisterSaveRoot(notnull EL_PersistenceComponent persistenceComponent, bool baked, bool autoSave)
	{
		m_mRootPersistenceComponents.Set(persistenceComponent, new Tuple2<bool, bool>(baked, autoSave));

		if(m_eState != EL_EPersistenceManagerState.WORLD_INIT)
		{
			m_pRootEntityCollection.Add(persistenceComponent, baked);
		}
	}
	
	void RegisterSaveRoot(notnull EL_PersistentScriptedStateBase scripedState, bool autoSave)
	{
		m_mRootScriptedStates.Set(scripedState, autoSave);
	}
	
	void UnregisterSaveRoot(notnull EL_PersistenceComponent persistenceComponent, bool baked)
	{
		m_mRootPersistenceComponents.Remove(persistenceComponent);
		m_pRootEntityCollection.Remove(persistenceComponent, baked);
	}
	
	void UnregisterSaveRoot(notnull EL_PersistentScriptedStateBase scripedState)
	{
		m_mRootScriptedStates.Remove(scripedState);
	}
	
	string GetPersistentId(notnull EL_PersistenceComponent persistenceComponent)
	{
		string id;
		
		if(m_eState == EL_EPersistenceManagerState.WORLD_INIT)
        {
			IEntity worldEntity = persistenceComponent.GetOwner();
            string name = worldEntity.GetName();
            if(!name)
            {
                Debug.Error(string.Format("Baked world entity '%1'@%2 needs to have a name to be trackable by the persistence system.", 
                    EL_Utils.GetPrefabName(worldEntity), 
                    worldEntity.GetOrigin()));
                return string.Empty;
            }
            
			id = m_pBakedEntityNameIdMapping.GetIdByName(name);
			
            if(!id)
            {
                id = EL_DbEntityIdGenerator.Generate();
                EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(worldEntity));
                m_pBakedEntityNameIdMapping.Insert(name, id, settings.m_tSaveDataTypename);
            }
			
			m_mBackedEntities.Set(id, worldEntity);
        }
		else if(m_sNextPersistentId)
		{
			id = m_sNextPersistentId;
		}
		else
		{
			id = EL_DbEntityIdGenerator.Generate();
		}
		
		return id;
	}
	
	string GetPersistentId(notnull EL_PersistentScriptedStateBase scripedState)
	{
		string id;
		
		if(m_sNextPersistentId)
		{
			id = m_sNextPersistentId;
		}
		else
		{
			id = EL_DbEntityIdGenerator.Generate();
		}
		
		return id;
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
