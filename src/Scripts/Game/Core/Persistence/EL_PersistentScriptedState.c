class EL_PersistentScriptedState
{
	private string m_sId;

	private int m_iLastSaved;

	[NonSerialized()]
	private EL_EPersistenceFlags m_eFlags;

	[NonSerialized()]
	private ref ScriptInvoker<EL_PersistentScriptedState, EL_ScriptedStateSaveData> m_pOnAfterSave;

	[NonSerialized()]
	private ref ScriptInvoker<EL_PersistentScriptedState, EL_ScriptedStateSaveData> m_pOnAfterPersist;

	[NonSerialized()]
	private ref ScriptInvoker<EL_PersistentScriptedState, EL_ScriptedStateSaveData> m_pOnBeforeLoad;

	[NonSerialized()]
	private ref ScriptInvoker<EL_PersistentScriptedState, EL_ScriptedStateSaveData> m_pOnAfterLoad;

	[NonSerialized()]
	private static ref map<EL_PersistentScriptedState, ref EL_ScriptedStateSaveData> m_mLastSaveData;

	//------------------------------------------------------------------------------------------------
	//! Get the assigned persistent id of this scripted state
	//! \return the id or empty string if persistence data is deleted and only the instance remains
	string GetPersistentId()
	{
		if (!m_sId) m_sId = EL_PersistenceManager.GetInstance().Register(this);
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Set the assigned persistent id of this scripted state.
	//! USE WITH CAUTION! Only in rare situations you need to manually assign an id.
	void SetPersistentId(string id)
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if (m_sId && m_sId != id)
		{
			persistenceManager.Unregister(this);
			m_sId = string.Empty;
		}
		if (!m_sId) m_sId = persistenceManager.Register(this, id);
	}

	//------------------------------------------------------------------------------------------------
	//! Get the last time this scripted state was saved as UTC unix timestamp
	int GetLastSaved()
	{
		return m_iLastSaved;
	}

	//------------------------------------------------------------------------------------------------
	//! Get internal state flags of the persistence tracking
	EL_EPersistenceFlags GetFlags()
	{
		return m_eFlags;
	}

	//------------------------------------------------------------------------------------------------
	//! Event invoker for when the save-data was read but was not yet persisted to the database.
	//! Args(EL_PersistentScriptedState, EL_ScriptedStateSaveData)
	ScriptInvoker GetOnAfterSaveEvent()
	{
		if (!m_pOnAfterSave) m_pOnAfterSave = new ScriptInvoker();
		return m_pOnAfterSave;
	}

	//------------------------------------------------------------------------------------------------
	//! Event invoker for when the save-data was persisted to the database.
	//! Args(EL_PersistentScriptedState, EL_ScriptedStateSaveData)
	ScriptInvoker GetOnAfterPersistEvent()
	{
		if (!m_pOnAfterPersist) m_pOnAfterPersist = new ScriptInvoker();
		return m_pOnAfterPersist;
	}

	//------------------------------------------------------------------------------------------------
	//! Event invoker for when the save-data is about to be loaded/applied to the cripted state.
	//! Args(EL_PersistentScriptedState, EL_ScriptedStateSaveData)
	ScriptInvoker GetOnBeforeLoadEvent()
	{
		if (!m_pOnBeforeLoad) m_pOnBeforeLoad = new ScriptInvoker();
		return m_pOnBeforeLoad;
	}

	//------------------------------------------------------------------------------------------------
	//! Event invoker for when the save-data was loaded/applied to the scripted state.
	//! Args(EL_PersistentScriptedState, EL_ScriptedStateSaveData)
	ScriptInvoker GetOnAfterLoadEvent()
	{
		if (!m_pOnAfterLoad) m_pOnAfterLoad = new ScriptInvoker();
		return m_pOnAfterLoad;
	}

	//------------------------------------------------------------------------------------------------
	//! Load existing save-data to apply to this scripted state
	bool Load(notnull EL_ScriptedStateSaveData saveData)
	{
		if (m_pOnBeforeLoad)
			m_pOnBeforeLoad.Invoke(this, saveData);

		SetPersistentId(saveData.GetId());

		Managed target;
		EL_PersistentScriptedStateProxy proxy = EL_PersistentScriptedStateProxy.Cast(this);
		if (proxy)
			target = proxy.m_pProxyTarget;

		if (!target)
		{
			target = this; // No proxy
		}
		else
		{
			// extract in case of proxy as "this" does not get save-data applied
			m_iLastSaved = saveData.m_iLastSaved;
		}

		if (!saveData.ApplyTo(target))
		{
			Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", saveData.Type().ToString(), saveData.GetId()));
			return false;
		}

		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(target.Type());
		if (EL_BitFlags.CheckFlags(settings.m_eOptions, EL_EPersistentScriptedStateOptions.USE_CHANGE_TRACKER))
			m_mLastSaveData.Set(this, saveData);

		if (m_pOnAfterLoad)
			m_pOnAfterLoad.Invoke(this, saveData);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Pause automated tracking for auto-/shutdown-save and removal.
	void PauseTracking()
	{
		EL_BitFlags.SetFlags(m_eFlags, EL_EPersistenceFlags.PAUSE_TRACKING);
	}

	//------------------------------------------------------------------------------------------------
	//! Undo PauseTracking().
	void ResumeTracking()
	{
		EL_BitFlags.ClearFlags(m_eFlags, EL_EPersistenceFlags.PAUSE_TRACKING);
	}

	//------------------------------------------------------------------------------------------------
	//! Save the scripted state to the database
	//! \return the save-data instance that was submitted to the database
	EL_ScriptedStateSaveData Save()
	{
		GetPersistentId(); // Make sure the id has been assigned

		m_iLastSaved = System.GetUnixTime();

		Managed target;
		EL_PersistentScriptedStateProxy proxy = EL_PersistentScriptedStateProxy.Cast(this);
		if (proxy)
			target = proxy.m_pProxyTarget;

		if (!target)
			target = this;

		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(target.Type());
		EL_ScriptedStateSaveData saveData = EL_ScriptedStateSaveData.Cast(settings.m_tSaveDataType.Spawn());
		if (!saveData || !saveData.ReadFrom(target))
		{
			Debug.Error(string.Format("Failed to persist scripted state '%1'. Save-data could not be read.", target));
			return null;
		}

		if (proxy)
		{
			saveData.SetId(m_sId);
			saveData.m_iLastSaved = m_iLastSaved;
		}

		if (m_pOnAfterSave)
			m_pOnAfterSave.Invoke(this, saveData);

		EL_ScriptedStateSaveData lastData;
		if (EL_BitFlags.CheckFlags(settings.m_eOptions, EL_EPersistentScriptedStateOptions.USE_CHANGE_TRACKER))
			lastData = m_mLastSaveData.Get(this);

		if (!lastData || !lastData.Equals(saveData))
			EL_PersistenceManager.GetInstance().GetDbContext().AddOrUpdateAsync(saveData);

		if (EL_BitFlags.CheckFlags(settings.m_eOptions, EL_EPersistentScriptedStateOptions.USE_CHANGE_TRACKER))
			m_mLastSaveData.Set(this, saveData);

		if (m_pOnAfterPersist)
			m_pOnAfterPersist.Invoke(this, saveData);

		return saveData;
	}

	//------------------------------------------------------------------------------------------------
	//! Delete the persistence data of this scripted state. Does not delete the state instance itself.
	void Delete()
	{
		// Only attempt to remove it if it was ever saved
		if (m_sId && m_iLastSaved > 0)
		{
			Managed target;
			EL_PersistentScriptedStateProxy proxy = EL_PersistentScriptedStateProxy.Cast(this);
			if (proxy)
				target = proxy.m_pProxyTarget;

			if (!target)
				target = this;

			EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(target.Type());
			EL_PersistenceManager.GetInstance().GetDbContext().RemoveAsync(settings.m_tSaveDataType, m_sId);
		}

		m_sId = string.Empty;
		m_iLastSaved = 0;
	}

	//------------------------------------------------------------------------------------------------
	void EL_PersistentScriptedState()
	{
		Managed target;
		EL_PersistentScriptedStateProxy proxy = EL_PersistentScriptedStateProxy.Cast(this);
		if (proxy)
		{
			// Yes this code is dirty as fuck, but no better way to not leak the properties into base class.
			// can't make them private or else they are not available in EL_PersistentScriptedStateProxy
			// and on protected they become available in inherited as intended for non proxies, which is also bad.
			// And I want to do the settings validation on ctor
			target = proxy.s_pNextProxyTarget;
			proxy.m_pProxyTarget = target;
			proxy.s_pNextProxyTarget = null;
		}

		if (!target)
			target = this;

		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(target.Type());
		if (!settings)
		{
			Debug.Error(string.Format("Missing settings annotation for scripted state type '%1'.", target.Type()));
			return;
		}

		if (!settings.m_tSaveDataType || settings.m_tSaveDataType == EL_ScriptedStateSaveData)
		{
			Debug.Error(string.Format("Missing or invalid save-data type on persistend scripted state '%1'. State will not be persisted!", target));
			return;
		}

		if (EL_BitFlags.CheckFlags(settings.m_eOptions, EL_EPersistentScriptedStateOptions.USE_CHANGE_TRACKER) && !m_mLastSaveData)
			m_mLastSaveData = new map<EL_PersistentScriptedState, ref EL_ScriptedStateSaveData>();

		EL_PersistenceManager.GetInstance().EnqueueRegistration(this);
	}

	//------------------------------------------------------------------------------------------------
	void ~EL_PersistentScriptedState()
	{
		if (m_mLastSaveData)
			m_mLastSaveData.Remove(this);

		// Check that we are not in session dtor phase.
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance(false);
		if (!persistenceManager || (persistenceManager.GetState() == EL_EPersistenceManagerState.SHUTDOWN)) return;

		persistenceManager.Unregister(this);

		if (EL_BitFlags.CheckFlags(m_eFlags, EL_EPersistenceFlags.PAUSE_TRACKING)) return;

		Managed target;
		EL_PersistentScriptedStateProxy proxy = EL_PersistentScriptedStateProxy.Cast(this);
		if (proxy)
			target = proxy.m_pProxyTarget;

		if (!target)
			target = this;

		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(target.Type());
		if (EL_BitFlags.CheckFlags(settings.m_eOptions, EL_EPersistentScriptedStateOptions.SELF_DELETE))
			Delete();
	}
};

class EL_PersistentScriptedStateProxy : EL_PersistentScriptedState
{
	static ref map<Managed, ref EL_PersistentScriptedStateProxy> s_mProxies;

	static Managed s_pNextProxyTarget;

	Managed m_pProxyTarget;

	//------------------------------------------------------------------------------------------------
	//! Create a proxy for a manged class you can't add a base class to.
	//! Will attempt restore last known state immediately
	//! \param targetInstance proxy target instance
	//! \param id Unique id to find save data for the target. Singleton is assumed if left empty.
	static EL_PersistentScriptedStateProxy Create(notnull Managed targetInstance, string id = string.Empty)
	{
		return CreateEx(targetInstance, id, false, null);
	}

	//------------------------------------------------------------------------------------------------
	//! Asynchronously Create a proxy for a manged class you can't add a base class to.
	//! Will attempt restore last known state but loading can take time.
	//! \param targetInstance proxy target instance
	//! \param id Unique id to find save data for the target. Singleton is assumed if left empty.
	//! \param callback Use the callback to handle the save-data load event
	static EL_PersistentScriptedStateProxy CreateAsync(notnull Managed targetInstance, string id = string.Empty, EL_DataCallbackSingle<EL_ScriptedStateSaveData> callback = null)
	{
		return CreateEx(targetInstance, id, true, callback);
	}

	//------------------------------------------------------------------------------------------------
	//!
	protected static EL_PersistentScriptedStateProxy CreateEx(
		notnull Managed targetInstance,
		string id,
		bool async,
		EL_DataCallbackSingle<EL_ScriptedStateSaveData> callback)
	{
		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(targetInstance.Type());
		if (!settings.m_tSaveDataType || targetInstance.IsInherited(EL_PersistentScriptedState))
			return null;

		s_pNextProxyTarget = targetInstance;
		EL_PersistentScriptedStateProxy proxy();

		EL_DbFindCondition condition;
		if (id)
			condition = EL_DbFind.Id().Equals(id);

		if (async)
		{
			Tuple2<EL_PersistentScriptedStateProxy, EL_DataCallbackSingle<EL_ScriptedStateSaveData>> context(proxy, callback);
			
			EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(
				settings.m_tSaveDataType,
				condition,
				limit: 1,
				callback: new EL_PersistentScriptedStateProxyCreateCallback(context: context));
		}
		else
		{
			array<ref EL_DbEntity> findResults = EL_PersistenceManager
				.GetInstance()
				.GetDbContext()
				.FindAll(settings.m_tSaveDataType, condition, limit: 1)
				.GetEntities();

			if (findResults && !findResults.IsEmpty())
			{
				EL_ScriptedStateSaveData saveData = EL_ScriptedStateSaveData.Cast(findResults.Get(0));
				if (saveData && !proxy.Load(saveData))
					return null;
			}
		}

		if (!s_mProxies)
			s_mProxies = new map<Managed, ref EL_PersistentScriptedStateProxy>();

		s_mProxies.Set(targetInstance, proxy);

		return proxy;
	}

	//------------------------------------------------------------------------------------------------
	//! Get proxy instance for the given target
	static EL_PersistentScriptedStateProxy Get(notnull Managed targetInstance)
	{
		if (!s_mProxies)
			return null;

		return s_mProxies.Get(targetInstance);
	}

	//------------------------------------------------------------------------------------------------
	//! Destory the proxy on destruction of the target instance to avoid any mem leaks.
	static void Destory(notnull Managed targetInstance)
	{
		if (s_mProxies)
			s_mProxies.Remove(targetInstance);
	}

	//------------------------------------------------------------------------------------------------
	//! Use Create instead
	protected void EL_PersistentScriptedStateProxy();
};

class EL_ScriptedStateSaveData : EL_MetaDataDbEntity
{
	//------------------------------------------------------------------------------------------------
	//! Reads the save-data from the scripted state
	//! \return EL_EReadResult.OK if save-data could be read, ERROR if something failed, DEFAULT if the data could be trimmed
	EL_EReadResult ReadFrom(notnull Managed scriptedState)
	{
		return EL_DbEntityUtils.StructAutoCopy(scriptedState, this);
	}

	//------------------------------------------------------------------------------------------------
	//! Instantiates the scripted state based on this save-data instance
	EL_PersistentScriptedState Spawn()
	{
		return EL_PersistenceManager.GetInstance().SpawnScriptedState(this);
	}

	//------------------------------------------------------------------------------------------------
	//! Applies the save-data to the scripted state
	EL_EApplyResult ApplyTo(notnull Managed scriptedState)
	{
		return EL_DbEntityUtils.StructAutoCopy(this, scriptedState);
	}

	//------------------------------------------------------------------------------------------------
	//! Compare scriped state save-data instances to see if there is any noteable difference
	//! \param other scriped state save-data to compare against
	//! \return true if save-data is considered to describe the same data. False on differences.
	bool Equals(notnull EL_ScriptedStateSaveData other)
	{
		return false;
	}
};

enum EL_EPersistentScriptedStateOptions
{
	USE_CHANGE_TRACKER	= 1,
	SELF_DELETE			= 2
};

class EL_PersistentScriptedStateSettings
{
	protected static ref map<typename, ref EL_PersistentScriptedStateSettings> s_mSettings;
	protected static ref map<typename, typename> s_mReverseMapping;

	typename m_tSaveDataType;
	EL_ESaveType m_eSaveType;
	EL_EPersistentScriptedStateOptions m_eOptions;

	//------------------------------------------------------------------------------------------------
	static EL_PersistentScriptedStateSettings Get(typename scriptedStateType)
	{
		if (!s_mSettings)
			return null;

		return s_mSettings.Get(scriptedStateType);
	}

	//------------------------------------------------------------------------------------------------
	static EL_PersistentScriptedStateSettings Get(Managed scriptedState)
	{
		return Get(scriptedState.Type());
	}

	//------------------------------------------------------------------------------------------------
	static typename GetScriptedStateType(typename saveDataType)
	{
		if (!s_mReverseMapping)
			return typename.Empty;

		return s_mReverseMapping.Get(saveDataType);
	}

	//------------------------------------------------------------------------------------------------
	void EL_PersistentScriptedStateSettings(
		typename scriptedStateType,
		EL_ESaveType saveType = EL_ESaveType.INTERVAL_SHUTDOWN,
		EL_EPersistentScriptedStateOptions options = 3) // Have to combine into nuber because enscript can't combine mask as const expression here ...
	{
		typename saveDataType = EL_ReflectionUtils.GetAttributeParent();

		if (!saveDataType.IsInherited(EL_ScriptedStateSaveData))
			Debug.Error(string.Format("Failed to register '%1' as persistence save struct for '%2'. '%1' must inherit from '%3'.", saveDataType, scriptedStateType, EL_ScriptedStateSaveData));

		if (!s_mSettings)
		{
			s_mSettings = new map<typename, ref EL_PersistentScriptedStateSettings>();
			s_mReverseMapping = new map<typename, typename>();
		}

		m_tSaveDataType = saveDataType;
		m_eSaveType = saveType;
		m_eOptions = options;

		EL_PersistentScriptedStateSettings existingSettings = s_mSettings.Get(scriptedStateType);
		if (existingSettings)
		{
			Debug.Error(string.Format("Failed to register '%1' as persistence save struct for '%2'. '%3' was already assigned to handle that type.", saveDataType, scriptedStateType, existingSettings.m_tSaveDataType));
			return;
		}

		s_mSettings.Set(scriptedStateType, this);
		s_mReverseMapping.Set(saveDataType, scriptedStateType);
	}
};

class EL_PersistentScriptedStateProxyCreateCallback : EL_DbFindCallbackSingle<EL_ScriptedStateSaveData>
{
	//------------------------------------------------------------------------------------------------
	override void OnSuccess(EL_ScriptedStateSaveData resultData, Managed context)
	{
		auto contextTyped = Tuple2<EL_PersistentScriptedStateProxy, EL_DataCallbackSingle<EL_ScriptedStateSaveData>>.Cast(context);
		
		if (resultData && contextTyped.param1.Load(resultData))
			contextTyped.param2.Invoke(resultData);
	}
};
