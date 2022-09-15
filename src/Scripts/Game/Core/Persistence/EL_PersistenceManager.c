enum EL_EPersistenceManagerState
{
	WORLD_INIT,
	SETUP,
	ACTIVE,
	SHUTDOWN
}

class EL_PersistenceManager
{
	protected static ref EL_PersistenceManager s_pInstance;

	protected ref EL_DbContext m_pDbContext;
	protected EL_EPersistenceManagerState m_eState;
	protected ref ScriptInvoker m_pOnStateChange;

	// Auto save system
	protected int m_iAutoSaveIterations;
	protected float m_fAutoSaveInterval;
	protected float m_fAutoSaveAccumultor;
	protected bool m_bAutoSaveActive;
	protected int m_iSaveOperation;
	protected MapIterator m_iAutoSaveEntityIt;
	protected MapIterator m_iAutoSaveScriptedStateIt;

	// Component init
	protected string m_sNextPersistentId;

	// GarbageManager handling
	protected ref EL_PersistentEntityLifetimeCollection m_pEntityLifetimeCollection;

	// Root instance tracking
	protected ref EL_PersistentRootEntityCollection m_pRootEntityCollection;
	protected ref map<EL_PersistenceComponent, bool> m_mRootPersistenceComponents;
	protected ref map<EL_PersistentScriptedStateBase, bool> m_mRootScriptedStates;
	protected ref set<string> m_aBackedEntityIds;

	// Only used during setup
	protected ref EL_PersistentBakedEntityNameIdMapping m_pBakedEntityNameIdMapping;
	protected ref map<string, ref EL_EntitySaveDataBase> m_mInitEntitySaveData;
	protected ref map<string, IEntity> m_mBackedEntities;

	// Find by id system
	protected ref map<string, IEntity> m_mAllEntities;
	protected ref map<string, EL_PersistentScriptedStateBase> m_mAllScriptedStates;

	//------------------------------------------------------------------------------------------------
	//! Check if current game instance is intended to run the persistence system. Only the mission host should do so.
	//! \return true if persistence should be run, false otherwise.
	static bool IsPersistenceMaster()
	{
		return GetGame().InPlayMode() && Replication.IsServer();
	}

	//------------------------------------------------------------------------------------------------
	protected void SetState(EL_EPersistenceManagerState state)
	{
		m_eState = state;
		if (m_pOnStateChange) m_pOnStateChange.Invoke(this, state);
	}

	//------------------------------------------------------------------------------------------------
	EL_EPersistenceManagerState GetState()
	{
		return m_eState;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the event invoker that can be subscribed to be notified about persistence manager state/phase changes.
	ScriptInvoker GetOnStateChangeEvent()
	{
		if (!m_pOnStateChange) m_pOnStateChange = new ScriptInvoker();
		return m_pOnStateChange;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the database context that is used by the persistence system
	//! \return database context instance
	EL_DbContext GetDbContext()
	{
		if (!m_pDbContext)
		{
			// TODO: Read persistence db source from server config.
			m_pDbContext = EL_DbContextFactory.GetContext();
		}

		return m_pDbContext;
	}

	//------------------------------------------------------------------------------------------------
	//! Find an entity that is registered in the persistence system by its persistent id
	//! \return entity instance or null if not found
	IEntity FindEntityByPersistentId(string persistentId)
	{
		return m_mAllEntities.Get(persistentId);
	}

	//------------------------------------------------------------------------------------------------
	//! Find a scripted state that is registered in the persistence system by its persistent id
	//! \return scripted state instance or null if not found
	EL_PersistentScriptedStateBase FindScriptedStateByPersistentId(string persistentId)
	{
		return m_mAllScriptedStates.Get(persistentId);
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsBaked(EL_PersistenceComponent persistenceComponent)
	{
		return m_aBackedEntityIds.Contains(persistenceComponent.GetPersistentId());
	}

	//------------------------------------------------------------------------------------------------
	//! Manually trigger the global auto-save. Resets the timer until the next auto-save cycle. If an auto-save is in progress it will do nothing.
	void AutoSave()
	{
		if (m_bAutoSaveActive) return;
		m_bAutoSaveActive = true;
		m_fAutoSaveAccumultor = 0;
		m_iSaveOperation = 0;
		m_iAutoSaveEntityIt = m_mRootPersistenceComponents.Begin();
		m_iAutoSaveScriptedStateIt = m_mRootScriptedStates.Begin();
	}

	//------------------------------------------------------------------------------------------------
	protected void AutoSaveTick()
	{
		if (!m_bAutoSaveActive) return;

		while (m_iAutoSaveEntityIt != m_mRootPersistenceComponents.End())
		{
			EL_PersistenceComponent persistenceComponent = m_mRootPersistenceComponents.GetIteratorKey(m_iAutoSaveEntityIt);
			bool autoSave = m_mRootPersistenceComponents.GetIteratorElement(m_iAutoSaveEntityIt);

			m_iAutoSaveEntityIt = m_mRootPersistenceComponents.Next(m_iAutoSaveEntityIt);

			if (!autoSave) continue; //Ignore if no auto-save

			UpdateRootEntityCollection(persistenceComponent);
			persistenceComponent.Save();
			m_iSaveOperation++;

			if ((m_eState == EL_EPersistenceManagerState.ACTIVE) &&
				((m_iSaveOperation + 1) % m_iAutoSaveIterations == 0))
			{
				return; // Pause execution until next tick
			}
		}

		while (m_iAutoSaveScriptedStateIt != m_mRootScriptedStates.End())
		{
			EL_PersistentScriptedStateBase scriptedState = m_mRootScriptedStates.GetIteratorKey(m_iAutoSaveScriptedStateIt);
			bool autoSaveEnabled = m_mRootScriptedStates.GetIteratorElement(m_iAutoSaveScriptedStateIt);

			m_iAutoSaveScriptedStateIt = m_mRootScriptedStates.Next(m_iAutoSaveScriptedStateIt);

			if (!autoSaveEnabled) continue; //Ignore if no auto-save

			scriptedState.Save();
			m_iSaveOperation++;

			if ((m_eState == EL_EPersistenceManagerState.ACTIVE) &&
				((m_iSaveOperation + 1) % m_iAutoSaveIterations == 0))
			{
				return; // Pause execution until next tick
			}
		}

		m_pRootEntityCollection.Save(GetDbContext());
		m_pEntityLifetimeCollection.Save(GetDbContext());
		m_bAutoSaveActive = false;
	}

	//------------------------------------------------------------------------------------------------
	protected void ShutDownSave()
	{
		foreach (EL_PersistenceComponent persistenceComponent, auto _ : m_mRootPersistenceComponents)
		{
			EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(persistenceComponent.GetOwner()));
			if (!settings.m_bShutdownsave) continue;

			UpdateRootEntityCollection(persistenceComponent);
			persistenceComponent.Save();
		}

		foreach (EL_PersistentScriptedStateBase scriptedState, auto _ : m_mRootScriptedStates)
		{
			if (!EL_PersistentScriptedStateSettings.Get(scriptedState.Type()).m_bShutDownSave) continue;

			scriptedState.Save();
		}

		m_pRootEntityCollection.Save(GetDbContext());
		m_pEntityLifetimeCollection.Save(GetDbContext());
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateRootEntityCollection(EL_PersistenceComponent persistenceComponent)
	{
		// Double check all root entites as some of them might have been stored or unstored without a manager noticing. Remove once ground item manipulation events are exposed.
		InventoryItemComponent inventoryItemComponent = InventoryItemComponent.Cast(persistenceComponent.GetOwner().FindComponent(InventoryItemComponent));
		if (!inventoryItemComponent) return;

		if (!inventoryItemComponent.GetParentSlot())
		{
			m_pRootEntityCollection.Add(persistenceComponent, IsBaked(persistenceComponent));
			m_pEntityLifetimeCollection.Add(persistenceComponent);
		}
		else
		{
			m_pRootEntityCollection.Remove(persistenceComponent, IsBaked(persistenceComponent));
			m_pEntityLifetimeCollection.Remove(persistenceComponent.GetPersistentId());
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void PrepareInitalWorldState()
	{
		// Remove baked entities that shall no longer be root entities in the world
		array<string> staleIds();
		foreach (string persistentId : m_pRootEntityCollection.m_aRemovedBackedEntities)
		{
			IEntity worldEntity = m_mBackedEntities.Get(persistentId);
			if (!worldEntity)
			{
				staleIds.Insert(persistentId);
				continue;
			}

			m_mBackedEntities.Remove(persistentId);
			Print(string.Format("EL_PersistenceManager::PrepareInitalWorldState() -> Deleting baked entity '%1'@%2.", EL_Utils.GetPrefabName(worldEntity), worldEntity.GetOrigin()), LogLevel.SPAM);
			SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
		}

		// Remove any removal entries for baked objects that no longer exist
		foreach (string staleId : staleIds)
		{
			int idx = m_pRootEntityCollection.m_aRemovedBackedEntities.Find(staleId);
			if (idx != -1) m_pRootEntityCollection.m_aRemovedBackedEntities.Remove(idx);
		}

		// Apply save-data to baked entities
		foreach (string persistentId, IEntity bakedEntity : m_mBackedEntities)
		{
			EL_EntitySaveDataBase saveData = m_mInitEntitySaveData.Get(persistentId);
			if (!saveData) continue;

			EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(bakedEntity.FindComponent(EL_PersistenceComponent));
			if (persistenceComponent && persistenceComponent.Load(saveData))
			{
				// Apply remaining lifetime
				m_pEntityLifetimeCollection.Apply(persistenceComponent);
			}
		}

		// Spawn additional entities from the previously bulk loaded data
		foreach (auto _, set<string> persistentIds : m_pRootEntityCollection.m_mSelfSpawnDynamicEntities)
		{
			foreach (string persistentId : persistentIds)
			{
				EL_EntitySaveDataBase saveData = m_mInitEntitySaveData.Get(persistentId);
				IEntity spawnedEntity = SpawnWorldEntity(saveData);
				if (spawnedEntity)
				{
					// Apply remaining lifetime
					EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(spawnedEntity.FindComponent(EL_PersistenceComponent));
					m_pEntityLifetimeCollection.Apply(persistenceComponent);
				}
			}
		}

		// Save any mapping or root entity changes detected during world init
		m_pBakedEntityNameIdMapping.Save(GetDbContext());
		if (staleIds.Count() > 0) m_pRootEntityCollection.Save(GetDbContext());

		// Free memory as it not needed after setup
		m_mInitEntitySaveData = null;
		m_pBakedEntityNameIdMapping = null;

		SetState(EL_EPersistenceManagerState.ACTIVE);
		Print("EL_PersistenceManager::PrepareInitalWorldState() -> Complete.");
	}

	//------------------------------------------------------------------------------------------------
	//! Used to spawn and correctly register an entity from save-data
	//! \param saveData Save-data to spawn from
	//! \return registered entiy instance or null on failure
	IEntity SpawnWorldEntity(EL_EntitySaveDataBase saveData)
	{
		if (!saveData || !saveData.GetId()) return null;

		Resource resource = Resource.Load(saveData.m_rPrefab);
		if (!resource.IsValid())
		{
			Debug.Error(string.Format("Invalid prefab type '%1' on '%2:%3' could not be spawned. Ignored.", saveData.m_rPrefab, saveData.Type().ToString(), saveData.GetId()));
			return null;
		}

		m_sNextPersistentId = saveData.GetId();
		IEntity worldEntity = GetGame().SpawnEntityPrefab(resource);
		if (!worldEntity)
		{
			Debug.Error(string.Format("Failed to spawn entity '%1:%2'. Ignored.", saveData.Type().ToString(), saveData.GetId()));
			return null;
		}

		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		if (!persistenceComponent || !persistenceComponent.Load(saveData))
		{
			SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
			return null;
		}

		return worldEntity;
	}

	//------------------------------------------------------------------------------------------------
	//! Used to instantiate and correctly register a scripted state from save-data
	//! \param saveData Save-data to instantiate from
	//! \return registered scripted state instance or null on failure
	EL_PersistentScriptedStateBase SpawnScriptedState(EL_ScriptedStateSaveDataBase saveData)
	{
		if (!saveData || !saveData.GetId()) return null;

		typename scriptedStateType = EL_PersistentScriptedStateSettings.GetScriptedStateType(saveData.Type());
		m_sNextPersistentId = saveData.GetId();
		EL_PersistentScriptedStateBase state = EL_PersistentScriptedStateBase.Cast(scriptedStateType.Spawn());
		if (!state)
		{
			Debug.Error(string.Format("Failed to spawn scripted state '%1:%2'. Ignored.", saveData.Type().ToString(), saveData.GetId()));
			return null;
		}

		if (!state.Load(saveData)) return null;

		return state;
	}

	//------------------------------------------------------------------------------------------------
	protected void LoadSetupData()
	{
		m_pBakedEntityNameIdMapping = EL_DbEntityHelper<EL_PersistentBakedEntityNameIdMapping>.GetRepository(GetDbContext()).FindSingleton().GetEntity();
		m_pEntityLifetimeCollection = EL_DbEntityHelper<EL_PersistentEntityLifetimeCollection>.GetRepository(GetDbContext()).FindSingleton().GetEntity();
		m_pRootEntityCollection = EL_DbEntityHelper<EL_PersistentRootEntityCollection>.GetRepository(GetDbContext()).FindSingleton().GetEntity();

		// Collect type and ids of inital world entities for bulk load
		map<typename, ref set<string>> bulkLoad();
		foreach (typename saveType, set<string> persistentIds: m_pRootEntityCollection.m_mSelfSpawnDynamicEntities)
		{
			set<string> loadIds();
			loadIds.Copy(persistentIds);
			bulkLoad.Set(saveType, loadIds);
		}
		foreach (string name, Tuple2<string, typename> idTypeTuple : m_pBakedEntityNameIdMapping.m_mNameIdMapping)
		{
			//Skip baked objects that will be deleted in load phase
			if (m_pRootEntityCollection.m_aRemovedBackedEntities.Contains(idTypeTuple.param1)) continue;

			set<string> loadIds = bulkLoad.Get(idTypeTuple.param2);

			if (!loadIds)
			{
				loadIds = new set<string>();
				bulkLoad.Set(idTypeTuple.param2, loadIds);
			}

			loadIds.Insert(idTypeTuple.param1);
		}

		// Load all known inital entity types from db, both baked and dynamic in one bulk operation
		foreach (typename saveDataType, set<string> persistentIds : bulkLoad)
		{
			array<string> loadIds();
			loadIds.Resize(persistentIds.Count());
			foreach (int idx, string id : persistentIds)
			{
				loadIds.Set(idx, id);
			}

			array<ref EL_DbEntity> findResults = GetDbContext().FindAll(saveDataType, EL_DbFind.Id().EqualsAnyOf(loadIds)).GetEntities();
			if (!findResults) continue;

			foreach (EL_DbEntity findResult : findResults)
			{
				EL_EntitySaveDataBase saveData = EL_EntitySaveDataBase.Cast(findResult);
				if (!saveData)
				{
					Debug.Error(string.Format("Unexpected database find result type '%1' encountered during entity load. Ignored.", findResult.Type().ToString()));
					continue;
				}

				m_mInitEntitySaveData.Set(saveData.GetId(), saveData);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Get the singleton instance of the persistence manager
	//! \param create Create the singleton if not yet existing
	//! \return persistence manager instance or null if game instance role does not allow persistence handling or the instance did not exist and create was disabled
	static EL_PersistenceManager GetInstance(bool create = true)
	{
		// Persistence logic only runs on the server machine
		if (!IsPersistenceMaster()) return null;

		if (!s_pInstance && create)
		{
			s_pInstance = new EL_PersistenceManagerInternal();

			//Reset the singleton when a new mission is loaded to free all memory and have a clean startup again.
			GetGame().m_OnMissionSetInvoker.Insert(Reset);
		}

		return s_pInstance;
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_PersistenceManager()
	{
		SetState(EL_EPersistenceManagerState.WORLD_INIT);

		m_mRootPersistenceComponents = new map<EL_PersistenceComponent, bool>();
		m_mRootScriptedStates = new map<EL_PersistentScriptedStateBase, bool>();
		m_aBackedEntityIds = new set<string>();

		m_mInitEntitySaveData = new map<string, ref EL_EntitySaveDataBase>();
		m_mBackedEntities = new map<string, IEntity>();

		m_mAllEntities = new map<string, IEntity>();
		m_mAllScriptedStates = new map<string, EL_PersistentScriptedStateBase>();

		LoadSetupData();
	}

	//------------------------------------------------------------------------------------------------
	protected static void Reset()
	{
		s_pInstance = null;
	}
}

class EL_PersistenceManagerInternal : EL_PersistenceManager
{
	//------------------------------------------------------------------------------------------------
	//! see GetInstance(bool), allows calls to internal functionality. Only use this if you really know what you are doing. No expected use case should need to call this function ever.
	static EL_PersistenceManagerInternal GetInternalInstance(bool create = true)
	{
		return EL_PersistenceManagerInternal.Cast(GetInstance(create));
	}

	//------------------------------------------------------------------------------------------------
	//! Set the db context manually, mainly used by testing framework
	void SetDbContext(notnull EL_DbContext dbContext)
	{
		m_pDbContext = dbContext;
	}

	//------------------------------------------------------------------------------------------------
	EL_PersistentRootEntityCollection GetRootEntityCollection()
	{
		return m_pRootEntityCollection;
	}

	//------------------------------------------------------------------------------------------------
	//! Register entity that is on the ground to start custom tracking for it
	void RegisterSaveRoot(notnull EL_PersistenceComponent persistenceComponent, bool autoSave)
	{
		m_mRootPersistenceComponents.Set(persistenceComponent, autoSave);

		// During world init, before the inital state was processed by PrepareInitalWorldState() ignore all baked entnties
		// otherwise they will add themselves back as known root entity reversing their yet pending possible removal.
		if (m_eState != EL_EPersistenceManagerState.WORLD_INIT)
		{
			m_pRootEntityCollection.Add(persistenceComponent, IsBaked(persistenceComponent));
		}

		m_pEntityLifetimeCollection.Add(persistenceComponent);
	}

	//------------------------------------------------------------------------------------------------
	//! Register known scripted states
	void RegisterSaveRoot(notnull EL_PersistentScriptedStateBase scripedState, bool autoSave)
	{
		m_mRootScriptedStates.Set(scripedState, autoSave);
	}

	//------------------------------------------------------------------------------------------------
	//! Unregister entity if deleted or stored inside a non ground item storage root
	void UnregisterSaveRoot(notnull EL_PersistenceComponent persistenceComponent)
	{
		m_mRootPersistenceComponents.Remove(persistenceComponent);
		m_pRootEntityCollection.Remove(persistenceComponent, IsBaked(persistenceComponent));
		m_pEntityLifetimeCollection.Remove(persistenceComponent.GetPersistentId());
	}

	//------------------------------------------------------------------------------------------------
	//! Unregister scripted state on its destruction
	void UnregisterSaveRoot(notnull EL_PersistentScriptedStateBase scripedState)
	{
		m_mRootScriptedStates.Remove(scripedState);
	}

	//------------------------------------------------------------------------------------------------
	//! Prepare the persistent entity id buffer so the next constructor of entity or scripted state will get assigned this id
	void SetNextPersistentId(string persistentId)
	{
		m_sNextPersistentId = persistentId;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the entity persistent id from the baked mapping, id buffer or generate a new one
	string GetPersistentId(notnull EL_PersistenceComponent persistenceComponent)
	{
		string id;

		if (m_eState == EL_EPersistenceManagerState.WORLD_INIT)
		{
			IEntity worldEntity = persistenceComponent.GetOwner();
			string name = worldEntity.GetName();
			if (!name) return string.Empty;

			id = m_pBakedEntityNameIdMapping.GetIdByName(name);

			if (!id)
			{
				id = EL_DbEntityIdGenerator.Generate();
				EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(worldEntity));
				m_pBakedEntityNameIdMapping.Insert(name, id, settings.m_tSaveDataTypename);
			}

			m_mBackedEntities.Set(id, worldEntity);
			m_aBackedEntityIds.Insert(id);
		}
		else if (m_sNextPersistentId)
		{
			id = m_sNextPersistentId;
			m_sNextPersistentId = string.Empty;
		}
		else
		{
			id = EL_DbEntityIdGenerator.Generate();
		}

		m_mAllEntities.Set(id, persistenceComponent.GetOwner());

		return id;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the scripted state persistent id from buffer or generate a new one
	string GetPersistentId(notnull EL_PersistentScriptedStateBase scripedState)
	{
		string id;

		if (m_sNextPersistentId)
		{
			id = m_sNextPersistentId;
			m_sNextPersistentId = string.Empty;
		}
		else
		{
			id = EL_DbEntityIdGenerator.Generate();
		}

		m_mAllScriptedStates.Set(id, scripedState);

		return id;
	}

	//------------------------------------------------------------------------------------------------
	//! Free any memory related to an entity or scripted state that is no longer part of the persistence system
	void UnloadPersistentId(string persistentId)
	{
		m_mAllEntities.Remove(persistentId);
		m_mAllScriptedStates.Remove(persistentId);
		m_pEntityLifetimeCollection.Remove(persistentId);
	}

	//------------------------------------------------------------------------------------------------
	event void OnPostInit(IEntity gameMode)
	{
		EL_PersistenceManagerComponent managerComponent = EL_PersistenceManagerComponent.Cast(gameMode.FindComponent(EL_PersistenceManagerComponent));
		EL_PersistenceManagerComponentClass settings = EL_PersistenceManagerComponentClass.Cast(managerComponent.GetComponentData(gameMode));

		if (settings.m_bEnabled)
		{
			m_fAutoSaveInterval = settings.m_fInterval;
			m_iAutoSaveIterations = Math.Clamp(settings.m_iIterations, 1, 128);
		}
	}

	//------------------------------------------------------------------------------------------------
	event void OnPostFrame(float timeSlice)
	{
		// Handle auto-save
		m_fAutoSaveAccumultor += timeSlice;

		if ((m_fAutoSaveInterval > 0) &&
			(m_fAutoSaveAccumultor >= m_fAutoSaveInterval))
		{
			AutoSave();
		}

		AutoSaveTick();
	}

	//------------------------------------------------------------------------------------------------
	event void OnWorldPostProcess(World world)
	{
		SetState(EL_EPersistenceManagerState.SETUP);
		GetGame().GetCallqueue().Call(PrepareInitalWorldState);
	}

	//------------------------------------------------------------------------------------------------
	event void OnGameEnd()
	{
		SetState(EL_EPersistenceManagerState.SHUTDOWN);
		ShutDownSave();
		Reset();
	}
}
