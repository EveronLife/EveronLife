enum EL_EPersistenceManagerState
{
	WORLD_INIT,
	SETUP,
	ACTIVE,
	SHUTDOWN
};

class EL_PersistenceManager
{
	protected static ref EL_PersistenceManager s_pInstance;

	// Startup and shutdown sequence
	protected EL_EPersistenceManagerState m_eState;
	protected ref ScriptInvoker m_pOnStateChange;

	// Underlying database connection
	protected ref EL_DbContext m_pDbContext;

	// Instance tracking
	protected ref array<EL_PersistenceComponent> m_aPendingEntityRegistrations;
	protected ref array<EL_PersistentScriptedState> m_aPendingScriptedStateRegistrations;
	protected ref EL_PersistentRootEntityCollection m_pRootEntityCollection;
	protected ref map<string, EL_PersistenceComponent> m_mRootAutoSave;
	protected ref map<string, typename> m_mRootAutoSaveCleanup;
	protected ref map<string, EL_PersistenceComponent> m_mRootShutdown;
	protected ref map<string, typename> m_mRootShutdownCleanup;
	protected ref map<string, EL_PersistenceComponent> m_mUncategorizedEntities;
	protected ref map<string, EL_PersistentScriptedState> m_mScriptedStateAutoSave;
	protected ref map<string, EL_PersistentScriptedState> m_mScriptedStateShutdown;
	protected ref map<string, EL_PersistentScriptedState> m_mScriptedStateUncategorized;

	// Auto save system
	protected int m_iAutoSaveIterations;
	protected float m_fAutoSaveInterval;
	protected float m_fAutoSaveAccumultor;
	protected bool m_bAutoSaveActive;
	protected int m_iSaveOperation;
	protected MapIterator m_iAutoSaveEntityIt;
	protected MapIterator m_iAutoSaveScriptedStateIt;

	// Setup buffers, discarded after world init
	protected ref map<string, EL_PersistenceComponent> m_mBakedRoots;
	protected ref EL_PersistentBakedEntityNameIdMapping m_pBakedEntityNameIdMapping;

	//------------------------------------------------------------------------------------------------
	//! Check if current game instance is intended to run the persistence system. Only the mission host should do so.
	//! \return true if persistence should be run, false otherwise.
	static bool IsPersistenceMaster()
	{
		return GetGame().InPlayMode() && Replication.IsServer();
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
			s_pInstance = new EL_PersistenceManager();

			//Reset the singleton when a new mission is loaded to free all memory and have a clean startup again.
			GetGame().m_OnMissionSetInvoker.Insert(Reset);
		}

		return s_pInstance;
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
	//! Used to spawn and correctly register an entity from save-data
	//! \param saveData Save-data to spawn from
	//! \param spawnAsSavedRoot true if the current record is a root entry in the db (not a stored item inside a storage)
	//! \return registered entiy instance or null on failure
	IEntity SpawnWorldEntity(EL_EntitySaveData saveData, bool spawnAsSavedRoot = true)
	{
		if (!saveData || !saveData.GetId()) return null;

		Resource resource = Resource.Load(saveData.m_rPrefab);
		if (!resource.IsValid())
		{
			Debug.Error(string.Format("Invalid prefab type '%1' on '%2:%3' could not be spawned. Ignored.", saveData.m_rPrefab, saveData.Type().ToString(), saveData.GetId()));
			return null;
		}

		IEntity entity = GetGame().SpawnEntityPrefab(resource);
		if (!entity)
		{
			Debug.Error(string.Format("Failed to spawn entity '%1:%2'. Ignored.", saveData.Type().ToString(), saveData.GetId()));
			return null;
		}

		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(entity.FindComponent(EL_PersistenceComponent));
		if (!persistenceComponent || !persistenceComponent.Load(saveData, spawnAsSavedRoot))
		{
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
			return null;
		}

		return entity;
	}

	//------------------------------------------------------------------------------------------------
	//! Used to instantiate and correctly register a scripted state from save-data
	//! \param saveData Save-data to instantiate from
	//! \return registered scripted state instance or null on failure
	EL_PersistentScriptedState SpawnScriptedState(EL_ScriptedStateSaveData saveData)
	{
		if (!saveData || !saveData.GetId()) return null;

		typename scriptedStateType = EL_PersistentScriptedStateSettings.GetScriptedStateType(saveData.Type());
		EL_PersistentScriptedState state = EL_PersistentScriptedState.Cast(scriptedStateType.Spawn());
		if (!state)
		{
			Debug.Error(string.Format("Failed to spawn scripted state '%1:%2'. Ignored.", saveData.Type().ToString(), saveData.GetId()));
			return null;
		}

		if (!state.Load(saveData)) return null;

		return state;
	}

	//------------------------------------------------------------------------------------------------
	//! Find an entity that is registered in the persistence system by its persistent id
	//! \return entity instance or null if not found
	IEntity FindEntityByPersistentId(string persistentId)
	{
		EL_PersistenceComponent result = FindPersistenceComponentById(persistentId);
		if (result) return result.GetOwner();
		return null;
	}

	//------------------------------------------------------------------------------------------------
	//! Find a peristence component that is registered in the persistence system by its id
	//! \return entity instance or null if not found
	EL_PersistenceComponent FindPersistenceComponentById(string persistentId)
	{
		FlushRegistrations();
		EL_PersistenceComponent result;
		if (m_mRootAutoSave.Find(persistentId, result)) return result;
		if (m_mRootShutdown.Find(persistentId, result)) return result;
		return m_mUncategorizedEntities.Get(persistentId);
	}

	//------------------------------------------------------------------------------------------------
	//! Find a scripted state that is registered in the persistence system by its persistent id
	//! \return scripted state instance or null if not found
	EL_PersistentScriptedState FindScriptedStateByPersistentId(string persistentId)
	{
		FlushRegistrations();
		EL_PersistentScriptedState result;
		if (m_mScriptedStateAutoSave.Find(persistentId, result)) return result;
		if (m_mScriptedStateShutdown.Find(persistentId, result)) return result;
		return m_mScriptedStateUncategorized.Get(persistentId);
	}

	//------------------------------------------------------------------------------------------------
	//! Manually trigger the global auto-save. Resets the timer until the next auto-save cycle. If an auto-save is already in progress it will do nothing.
	void AutoSave()
	{
		if (m_bAutoSaveActive) return;
		m_bAutoSaveActive = true;
		m_fAutoSaveAccumultor = 0;
		m_iSaveOperation = 0;
		m_iAutoSaveEntityIt = m_mRootAutoSave.Begin();
		m_iAutoSaveScriptedStateIt = m_mScriptedStateAutoSave.Begin();
		FlushRegistrations();
	}

	//------------------------------------------------------------------------------------------------
	protected void AutoSaveTick()
	{
		if (!m_bAutoSaveActive) return;

		while (m_iAutoSaveEntityIt != m_mRootAutoSave.End())
		{
			EL_PersistenceComponent persistenceComponent = m_mRootAutoSave.GetIteratorElement(m_iAutoSaveEntityIt);
			m_iAutoSaveEntityIt = m_mRootAutoSave.Next(m_iAutoSaveEntityIt);

			if (EL_BitFlags.CheckFlags(persistenceComponent.GetFlags(), EL_EPersistenceFlags.PAUSE_TRACKING)) continue;

			persistenceComponent.Save();
			m_iSaveOperation++;

			if ((m_eState == EL_EPersistenceManagerState.ACTIVE) &&
				((m_iSaveOperation + 1) % m_iAutoSaveIterations == 0))
			{
				return; // Pause execution until next tick
			}
		}

		while (m_iAutoSaveScriptedStateIt != m_mScriptedStateAutoSave.End())
		{
			EL_PersistentScriptedState scriptedState = m_mScriptedStateAutoSave.GetIteratorElement(m_iAutoSaveScriptedStateIt);
			m_iAutoSaveScriptedStateIt = m_mScriptedStateAutoSave.Next(m_iAutoSaveScriptedStateIt);

			if (EL_BitFlags.CheckFlags(scriptedState.GetFlags(), EL_EPersistenceFlags.PAUSE_TRACKING)) continue;

			scriptedState.Save();
			m_iSaveOperation++;

			if ((m_eState == EL_EPersistenceManagerState.ACTIVE) &&
				((m_iSaveOperation + 1) % m_iAutoSaveIterations == 0))
			{
				return; // Pause execution until next tick
			}
		}

		m_pRootEntityCollection.Save(GetDbContext());

		// Remove records about former root enties that were not purged by a persistent parent's recursive save.
		foreach (string persistentId, typename saveDataTypename : m_mRootAutoSaveCleanup)
		{
			GetDbContext().RemoveAsync(saveDataTypename, persistentId);
		}
		m_mRootAutoSaveCleanup.Clear();

		m_bAutoSaveActive = false;
	}

	//------------------------------------------------------------------------------------------------
	protected void ShutDownSave()
	{
		FlushRegistrations();

		foreach (auto _, EL_PersistenceComponent persistenceComponent : m_mRootShutdown)
		{
			if (EL_BitFlags.CheckFlags(persistenceComponent.GetFlags(), EL_EPersistenceFlags.PAUSE_TRACKING)) continue;
			persistenceComponent.Save();
		}

		foreach (auto _, EL_PersistentScriptedState scriptedState : m_mScriptedStateShutdown)
		{
			if (EL_BitFlags.CheckFlags(scriptedState.GetFlags(), EL_EPersistenceFlags.PAUSE_TRACKING)) continue;
			scriptedState.Save();
		}

		m_pRootEntityCollection.Save(GetDbContext());

		// Remove records about former root enties that were not purged by a persistent parent's recursive save.
		foreach (string persistentId, typename saveDataTypename : m_mRootShutdownCleanup)
		{
			GetDbContext().RemoveAsync(saveDataTypename, persistentId);
		}
		m_mRootShutdownCleanup.Clear();
	}

	//------------------------------------------------------------------------------------------------
	protected void PrepareInitalWorldState()
	{
		SetState(EL_EPersistenceManagerState.SETUP);

		// Remove baked entities that shall no longer be root entities in the world
		array<string> staleIds();
		foreach (string persistentId : m_pRootEntityCollection.m_aRemovedBackedRootEntities)
		{
			IEntity entity = FindEntityByPersistentId(persistentId);
			if (!entity)
			{
				staleIds.Insert(persistentId);
				continue;
			}

			Print(string.Format("EL_PersistenceManager::PrepareInitalWorldState() -> Deleting baked entity '%1'@%2.", EL_Utils.GetPrefabName(entity), entity.GetOrigin()), LogLevel.SPAM);
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
		}

		// Remove any removal entries for baked objects that no longer exist
		foreach (string staleId : staleIds)
		{
			m_pRootEntityCollection.m_aRemovedBackedRootEntities.RemoveItem(staleId);
		}

		// Collect type and ids of inital world entities for bulk load
		map<typename, ref array<string>> bulkLoad();
		foreach (typename saveType, array<string> persistentIds : m_pRootEntityCollection.m_mSelfSpawnDynamicEntities)
		{
			array<string> loadIds();
			loadIds.Copy(persistentIds);
			bulkLoad.Set(saveType, loadIds);
		}
		foreach (string id, EL_PersistenceComponent persistenceComponent : m_mBakedRoots)
		{
			EL_PersistenceComponentClass settings = EL_ComponentData<EL_PersistenceComponentClass>.Get(persistenceComponent);
			array<string> loadIds = bulkLoad.Get(settings.m_tSaveDataTypename);

			if (!loadIds)
			{
				loadIds = {};
				bulkLoad.Set(settings.m_tSaveDataTypename, loadIds);
			}

			loadIds.Insert(id);
		}

		// Load all known inital entity types from db, both baked and dynamic in one bulk operation
		foreach (typename saveDataType, array<string> persistentIds : bulkLoad)
		{
			array<ref EL_DbEntity> findResults = GetDbContext().FindAll(saveDataType, EL_DbFind.Id().EqualsAnyOf(persistentIds)).GetEntities();
			if (!findResults) continue;

			foreach (EL_DbEntity findResult : findResults)
			{
				EL_EntitySaveData saveData = EL_EntitySaveData.Cast(findResult);
				if (!saveData)
				{
					Debug.Error(string.Format("Unexpected database find result type '%1' encountered during entity load. Ignored.", findResult.Type().ToString()));
					continue;
				}

				// Load data for baked roots
				EL_PersistenceComponent persistenceComponent = m_mBakedRoots.Get(saveData.GetId());
				if (persistenceComponent)
				{
					persistenceComponent.Load(saveData);
					continue;
				}

				// Spawn additional dynamic entites
				SpawnWorldEntity(saveData);
			}
		}

		// Save any mapping or root entity changes detected during world init
		EL_DbContext dbContext = GetDbContext();
		m_pBakedEntityNameIdMapping.Save(dbContext);
		m_pRootEntityCollection.Save(dbContext);

		// Free memory as it not needed after setup
		m_mBakedRoots = null;
		m_pBakedEntityNameIdMapping = null;

		Print("EL_PersistenceManager::PrepareInitalWorldState() -> Complete.", LogLevel.VERBOSE);
		SetState(EL_EPersistenceManagerState.ACTIVE);
	}

	//------------------------------------------------------------------------------------------------
	//! Get the entity persistent id from the baked mapping, id buffer or generate a new one
	protected string GetPersistentId(notnull EL_PersistenceComponent persistenceComponent)
	{
		string id;

		if (m_eState < EL_EPersistenceManagerState.SETUP)
		{
			IEntity entity = persistenceComponent.GetOwner();
			string name = entity.GetName();
			if (!name)
			{
				IEntity parent = entity.GetParent();
				while (parent && !parent.GetName())
				{
					parent = parent.GetParent()
				}

				if (parent && parent.GetName())
				{
					string namePart = string.Format("%1_%2", parent.GetName(), EL_Utils.GetPrefabName(entity).Substring(1, 16));
					int duplicate;
					while (true)
					{
						name = string.Format("%1_%2", namePart, duplicate++);
						// Make sure the name is not already taken, otherwise increment counter of same child prefab type
						if (m_pBakedEntityNameIdMapping.m_aBakedNames.Insert(name))
							break;
					}

				}
				else
				{
					Debug.Error(string.Format("Unnamed baked parent '%1'. Unable to assign persistent id from name<->id mapping. Check your world setup!", EL_Utils.GetPrefabName(entity.GetParent())));
					return string.Empty;
				}
			}

			id = m_pBakedEntityNameIdMapping.GetIdByName(name);

			if (!id)
			{
				id = EL_DbEntityIdGenerator.Generate();
				EL_PersistenceComponentClass settings = EL_ComponentData<EL_PersistenceComponentClass>.Get(persistenceComponent);
				m_pBakedEntityNameIdMapping.Insert(name, id, settings.m_tSaveDataTypename);
			}
		}
		else
		{
			id = EL_DbEntityIdGenerator.Generate();
		}

		return id;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the scripted state persistent id from buffer or generate a new one
	protected string GetPersistentId(notnull EL_PersistentScriptedState scripedState)
	{
		return EL_DbEntityIdGenerator.Generate();
	}

	//------------------------------------------------------------------------------------------------
	//! Set the db context manually, mainly used by testing framework
	void SetDbContext(notnull EL_DbContext dbContext)
	{
		m_pDbContext = dbContext;
	}

	//------------------------------------------------------------------------------------------------
	//! Enqueue entity for persistence registration for later (will be flushed before any find by id or save operation takes place)
	void EnqueueRegistration(notnull EL_PersistenceComponent persistenceComponent)
	{
		m_aPendingEntityRegistrations.Insert(persistenceComponent);
	}

	//------------------------------------------------------------------------------------------------
	//! Register the persistence component to assign it's id and make it searchable.
	string Register(notnull EL_PersistenceComponent persistenceComponent, string id = string.Empty)
	{
		IEntity owner = persistenceComponent.GetOwner();
		if (!owner) return string.Empty;

		if (!id) id = GetPersistentId(persistenceComponent);

		EL_PersistenceComponentClass settings = EL_ComponentData<EL_PersistenceComponentClass>.Get(persistenceComponent);
		bool isRoot = settings.m_bStorageRoot && EL_BitFlags.CheckFlags(persistenceComponent.GetFlags(), EL_EPersistenceFlags.STORAGE_ROOT);
		UpdateRootStatus(persistenceComponent, id, settings.m_eSaveType, settings.m_tSaveDataTypename, isRoot);

		return id;
	}

	//------------------------------------------------------------------------------------------------
	void UpdateRootStatus(notnull EL_PersistenceComponent persistenceComponent, string id, EL_ESaveType saveType, typename saveDataType, bool isRootEntity)
	{
		switch (saveType)
		{
			case EL_ESaveType.INTERVAL_SHUTDOWN:
			{
				if (isRootEntity)
				{
					m_mUncategorizedEntities.Remove(id);
					m_mRootAutoSaveCleanup.Remove(id);
					m_mRootAutoSave.Set(id, persistenceComponent);
				}
				else
				{
					if (m_mRootAutoSave.Contains(id))
					{
						m_mRootAutoSave.Remove(id);

						if (EL_BitFlags.CheckFlags(persistenceComponent.GetFlags(), EL_EPersistenceFlags.PERSISTENT_RECORD))
							m_mRootAutoSaveCleanup.Set(id, saveDataType);
					}
					m_mUncategorizedEntities.Set(id, persistenceComponent);
				}
				break;
			}

			case EL_ESaveType.SHUTDOWN:
			{
				if (isRootEntity)
				{
					m_mUncategorizedEntities.Remove(id);
					m_mRootShutdownCleanup.Remove(id);
					m_mRootShutdown.Set(id, persistenceComponent);
				}
				else
				{
					if (m_mRootShutdown.Contains(id))
					{
						m_mRootShutdown.Remove(id);

						if (EL_BitFlags.CheckFlags(persistenceComponent.GetFlags(), EL_EPersistenceFlags.PERSISTENT_RECORD))
							m_mRootShutdownCleanup.Set(id, saveDataType);
					}
					m_mUncategorizedEntities.Set(id, persistenceComponent);
				}
				break;
			}

			default:
			{
				m_mUncategorizedEntities.Set(id, persistenceComponent);
				break;
			}
		}

		UpdateRootEntityCollection(persistenceComponent, id, isRootEntity);
	}

	//------------------------------------------------------------------------------------------------
	void UpdateRootEntityCollection(notnull EL_PersistenceComponent persistenceComponent, string id, bool isRootEntity)
	{
		if (isRootEntity)
		{
			m_pRootEntityCollection.Add(persistenceComponent, id, m_eState);

			if (m_mBakedRoots && EL_BitFlags.CheckFlags(persistenceComponent.GetFlags(), EL_EPersistenceFlags.BAKED_ROOT))
			{
				m_mBakedRoots.Set(id, persistenceComponent);
			}
		}
		else
		{
			m_pRootEntityCollection.Remove(persistenceComponent, id, m_eState);
			if (m_mBakedRoots) m_mBakedRoots.Remove(id);
		}
	}

	//------------------------------------------------------------------------------------------------
	void ForceSelfSpawn(notnull EL_PersistenceComponent persistenceComponent)
	{
		EL_PersistenceComponentClass settings = EL_ComponentData<EL_PersistenceComponentClass>.Get(persistenceComponent);
		m_pRootEntityCollection.ForceSelfSpawn(persistenceComponent, persistenceComponent.GetPersistentId(), settings);
	}

	//------------------------------------------------------------------------------------------------
	void Unregister(notnull EL_PersistenceComponent persistenceComponent)
	{
		string id = persistenceComponent.GetPersistentId();
		m_mRootAutoSave.Remove(id);
		m_mRootShutdown.Remove(id);
		m_mUncategorizedEntities.Remove(id);
	}

	//------------------------------------------------------------------------------------------------
	void AddOrUpdateAsync(notnull EL_EntitySaveData saveData)
	{
		GetDbContext().AddOrUpdateAsync(saveData);
	}

	//------------------------------------------------------------------------------------------------
	void RemoveAsync(typename saveDataType, string id)
	{
		m_mRootAutoSaveCleanup.Remove(id);
		m_mRootShutdownCleanup.Remove(id);
		GetDbContext().RemoveAsync(saveDataType, id);
	}

	//------------------------------------------------------------------------------------------------
	void EnqueueRemoval(typename saveDataType, string id, EL_ESaveType saveType)
	{
		if (saveType == EL_ESaveType.MANUAL)
		{
			RemoveAsync(saveDataType, id);
			return;
		}

		if (saveType == EL_ESaveType.INTERVAL_SHUTDOWN)
		{
			m_mRootAutoSaveCleanup.Set(id, saveDataType);
			return;
		}

		m_mRootShutdownCleanup.Set(id, saveDataType);
	}

	//------------------------------------------------------------------------------------------------
	//! Enqueue scripted stat for persistence registration for later (will be flushed before any find by id or save operation takes place)
	void EnqueueRegistration(notnull EL_PersistentScriptedState scripedState)
	{
		m_aPendingScriptedStateRegistrations.Insert(scripedState);
	}

	//------------------------------------------------------------------------------------------------
	//! Register the scripted state to assign it's id, subscribe to save events and make it searchable.
	string Register(notnull EL_PersistentScriptedState scripedState, string id = string.Empty)
	{
		if (!id) id = GetPersistentId(scripedState);

		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(scripedState);

		if (settings.m_eSaveType == EL_ESaveType.INTERVAL_SHUTDOWN)
		{
			m_mScriptedStateAutoSave.Set(id, scripedState);
		}
		else if (settings.m_eSaveType == EL_ESaveType.SHUTDOWN)
		{
			m_mScriptedStateShutdown.Set(id, scripedState);
		}
		else
		{
			m_mScriptedStateUncategorized.Set(id, scripedState);
		}

		return id;
	}

	//------------------------------------------------------------------------------------------------
	void Unregister(notnull EL_PersistentScriptedState scripedState)
	{
		string id = scripedState.GetPersistentId();
		m_mScriptedStateAutoSave.Remove(id);
		m_mScriptedStateShutdown.Remove(id);
		m_mScriptedStateUncategorized.Remove(id);
	}

	//------------------------------------------------------------------------------------------------
	protected void FlushRegistrations()
	{
		// Ask for persistent ids. If they were already registerd, they will have them, if not they are registered now.

		foreach (EL_PersistenceComponent persistenceComponent : m_aPendingEntityRegistrations)
		{
			if (persistenceComponent) persistenceComponent.GetPersistentId();
		}
		m_aPendingEntityRegistrations.Clear();

		foreach (EL_PersistentScriptedState scripedState : m_aPendingScriptedStateRegistrations)
		{
			if (scripedState) scripedState.GetPersistentId();
		}
		m_aPendingScriptedStateRegistrations.Clear();
	}

	//------------------------------------------------------------------------------------------------
	event void OnPostInit(IEntity gameMode)
	{
		EL_PersistenceManagerComponent managerComponent = EL_Component<EL_PersistenceManagerComponent>.Find(gameMode);
		EL_PersistenceManagerComponentClass settings = EL_ComponentData<EL_PersistenceManagerComponentClass>.Get(managerComponent);
		if (!settings.m_bEnabled) return;
		m_fAutoSaveInterval = settings.m_fInterval;
		m_iAutoSaveIterations = Math.Clamp(settings.m_iIterations, 1, 128);
	}

	//------------------------------------------------------------------------------------------------
	event void OnPostFrame(float timeSlice)
	{
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
		FlushRegistrations();
		PrepareInitalWorldState();
	}

	//------------------------------------------------------------------------------------------------
	event void OnGameEnd()
	{
		SetState(EL_EPersistenceManagerState.SHUTDOWN);
		AutoSave(); // Trigger auto-save
		AutoSaveTick(); // Execute auto-save instantly till end
		ShutDownSave(); // Save those who only save on shutdown
		Reset();
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_PersistenceManager()
	{
		SetState(EL_EPersistenceManagerState.WORLD_INIT);

		m_aPendingEntityRegistrations = {};
		m_aPendingScriptedStateRegistrations = {};
		m_mRootAutoSave = new map<string, EL_PersistenceComponent>();
		m_mRootAutoSaveCleanup = new map<string, typename>();
		m_mRootShutdown = new map<string, EL_PersistenceComponent>();
		m_mRootShutdownCleanup = new map<string, typename>();
		m_mUncategorizedEntities = new map<string, EL_PersistenceComponent>();
		m_mScriptedStateAutoSave = new map<string, EL_PersistentScriptedState>();
		m_mScriptedStateShutdown = new map<string, EL_PersistentScriptedState>();
		m_mScriptedStateUncategorized = new map<string, EL_PersistentScriptedState>();

		m_mBakedRoots = new map<string, EL_PersistenceComponent>();
		m_pBakedEntityNameIdMapping = EL_DbEntityHelper<EL_PersistentBakedEntityNameIdMapping>.GetRepository(GetDbContext()).FindSingleton().GetEntity();
		m_pRootEntityCollection = EL_DbEntityHelper<EL_PersistentRootEntityCollection>.GetRepository(GetDbContext()).FindSingleton().GetEntity();
	}

	//------------------------------------------------------------------------------------------------
	protected static void Reset()
	{
		EL_DefaultPrefabItemsInfo.Reset();
		EL_EntitySlotPrefabInfo.Reset();
		s_pInstance = null;
	}
};
