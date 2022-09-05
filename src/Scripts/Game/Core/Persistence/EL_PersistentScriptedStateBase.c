class EL_PersistentScriptedStateBase
{
	protected string m_sId;
	protected EL_DateTimeUtcAsInt m_iLastSaved;

	protected bool m_bDetatched;

	//------------------------------------------------------------------------------------------------
	//! Get the assigned persistent id of this scripted state
	//! \return the id or empty string if persistence data is deleted and only the instance remains
	string GetPersistentId()
	{
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the last time this scripted state was saved as packed UTC date time
	//! \return utc date time represented as packed integer
	EL_DateTimeUtcAsInt GetLastSaved()
	{
		return m_iLastSaved;
	}

	//------------------------------------------------------------------------------------------------
	//! Save the scripted state to the database
	//! \return the save-data instance that was submitted to the database
	EL_ScriptedStateSaveDataBase Save()
	{
		if (m_bDetatched || !m_sId) return null;

		m_iLastSaved = EL_DateTimeUtcAsInt.Now();

		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(Type());
		EL_ScriptedStateSaveDataBase saveData = EL_ScriptedStateSaveDataBase.Cast(settings.m_tSaveDataType.Spawn());
		if (!saveData || !saveData.ReadFrom(this))
		{
			Debug.Error(string.Format("Failed to persist scripted state '%1'. Save-data could not be read.", this));
			return null;
		}

		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);

		return saveData;
	}

	//------------------------------------------------------------------------------------------------
	//! Load existing save-data to apply to this scripted state
	bool Load(notnull EL_ScriptedStateSaveDataBase saveData)
	{
		if (m_bDetatched || !saveData.GetId() || !saveData.ApplyTo(this))
		{
			Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", saveData.Type().ToString(), saveData.GetId()));
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Delete the persistence data of this scripted state. Does not delete the state instance itself.
	void Delete()
	{
		if (m_bDetatched || !m_sId) return;

		if (m_iLastSaved > 0)
		{
			// Only attempt to remove it if it was ever saved
			EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
			EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(Type());
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataType, m_sId);
		}

		m_sId = string.Empty;
		m_iLastSaved = 0;
	}

	//------------------------------------------------------------------------------------------------
	//! Mark the scriped state as detached from persistence, to ignore Save, Load and Delete operations. Can not be undone. Used primarily to handle removal of the instance externally.
	void Detach()
	{
		m_bDetatched = true;
	}

	//------------------------------------------------------------------------------------------------
	void EL_PersistentScriptedStateBase()
	{
		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(Type());
		if (!settings)
		{
			Debug.Error(string.Format("Missing settings annotation for scripted state type '%1'.", Type()));
			return;
		}

		if (!settings.m_tSaveDataType || settings.m_tSaveDataType == EL_ScriptedStateSaveDataBase)
		{
			Debug.Error(string.Format("Missing or invalid save-data type on persistend scripted state '%1'. State will not be persisted!", this));
			return;
		}

		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		m_sId = persistenceManager.GetPersistentId(this);
		persistenceManager.RegisterSaveRoot(this, settings.m_bAutosave);
	}

	//------------------------------------------------------------------------------------------------
	void ~EL_PersistentScriptedStateBase()
	{
		// Check that we are not in session dtor phase.
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance(false);
		if (!persistenceManager || (persistenceManager.GetState() == EL_EPersistenceManagerState.SHUTDOWN)) return;

		persistenceManager.UnregisterSaveRoot(this);
		persistenceManager.UnloadPersistentId(m_sId);

		// Only auto self delete if setting for it is enabled
		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(Type());
		if (!settings.m_bSelfDelete) return;

		Delete();
	}
}

class EL_ScriptedStateSaveDataBase : EL_DbEntity
{
	int m_iDataLayoutVersion = 1;
	EL_DateTimeUtcAsInt m_iLastSaved;

	//------------------------------------------------------------------------------------------------
	//! Reads the save-data from the scripted state
	bool ReadFrom(notnull EL_PersistentScriptedStateBase scriptedState)
	{
		return EL_DbEntityUtils.StructAutoCopy(scriptedState, this);
	}

	//------------------------------------------------------------------------------------------------
	//! Instantiates the scripted state based on this save-data instance
	EL_PersistentScriptedStateBase Spawn()
	{
		return EL_PersistenceManager.GetInstance().SpawnScriptedState(this);
	}

	//------------------------------------------------------------------------------------------------
	//! Applies the save-data to the scripted state
	bool ApplyTo(notnull EL_PersistentScriptedStateBase scriptedState)
	{
		return EL_DbEntityUtils.StructAutoCopy(this, scriptedState);
	}
}

class EL_PersistentScriptedStateSettings
{
	protected static ref map<typename, ref EL_PersistentScriptedStateSettings> s_mSettings;
	protected static ref map<typename, typename> s_mReverseMapping;

	typename m_tSaveDataType;
	bool m_bAutosave;
	bool m_bShutDownSave;
	bool m_bSelfDelete;

	//------------------------------------------------------------------------------------------------
	static EL_PersistentScriptedStateSettings Get(typename scriptedStateType)
	{
		if (!s_mSettings) return null;

		return s_mSettings.Get(scriptedStateType);
	}

	//------------------------------------------------------------------------------------------------
	static typename GetScriptedStateType(typename saveDataType)
	{
		if (!s_mReverseMapping) return typename.Empty;

		return s_mReverseMapping.Get(saveDataType);
	}

	//------------------------------------------------------------------------------------------------
	void EL_PersistentScriptedStateSettings(typename scriptedStateType, typename saveDataType, bool autoSave = true, bool shutDownSave = true, bool selfDelete = true)
	{
		if (!saveDataType.IsInherited(EL_ScriptedStateSaveDataBase))
		{
			Debug.Error(string.Format("Failed to register '%1' as persistence save struct for '%2'. '%1' must inherit from '%3'.", saveDataType, scriptedStateType, EL_ScriptedStateSaveDataBase));
		}

		if (!s_mSettings)
		{
			s_mSettings = new map<typename, ref EL_PersistentScriptedStateSettings>();
			s_mReverseMapping = new map<typename, typename>();
		}

		m_tSaveDataType = saveDataType;
		m_bAutosave = autoSave;
		m_bShutDownSave = shutDownSave;
		m_bSelfDelete = selfDelete;

		s_mSettings.Set(scriptedStateType, this);
		s_mReverseMapping.Set(saveDataType, scriptedStateType);
	}
}
