class EL_PersistentScriptedStateBase
{
	protected string m_sId;
	protected EL_DateTimeUtcAsInt m_iLastSaved;
	
	protected bool m_bDetatched;
	
	string GetPersistentId()
	{
		if(!m_sId) m_sId = EL_PersistenceManagerInternal.GetInternalInstance().GetPersistentId(this);
		
		return m_sId;
	}
	
	EL_DateTimeUtcAsInt GetLastSaved()
	{
		return m_iLastSaved;
	}
	
	EL_ScriptedStateSaveDataBase Save()
	{
		if(!GetPersistentId()) return null;
		
		m_iLastSaved = EL_DateTimeUtcAsInt.Now();
		
		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(Type());
		EL_ScriptedStateSaveDataBase saveData = EL_ScriptedStateSaveDataBase.Cast(settings.m_tSaveDataType.Spawn());		
		if(!saveData || !saveData.ReadFrom(this))
		{
			Debug.Error(string.Format("Failed to persist scripted state '%1'. Save-data could not be read.", this));
			return null;
		}
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
		
		return saveData;
	}
	
	bool Load(notnull EL_ScriptedStateSaveDataBase saveData)
	{
		if(!saveData.GetId() || !saveData.ApplyTo(this))
		{
			Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", saveData.Type().ToString(), saveData.GetId()));
			return false;
		}
		
		return true;
	}
	
	void Delete()
	{
		if(!m_sId) return;

		if(m_iLastSaved > 0)
		{
			// Only attempt to remove it if it was ever saved
			EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
			EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(Type());
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataType, m_sId);
		}
		
		m_sId = string.Empty;
		m_iLastSaved = 0;
	}
	
	void Detach()
	{
		m_bDetatched = true;
	}
	
	void EL_PersistentScriptedStateBase()
	{
		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(Type());
		if(!settings)
		{
			Debug.Error(string.Format("Missing settings annotation for scripted state type '%1'.", Type()));
			return;
		}
		
		if(!settings.m_tSaveDataType || settings.m_tSaveDataType == EL_ScriptedStateSaveDataBase)
		{
			Debug.Error(string.Format("Missing or invalid save-data type on persistend scripted state '%1'. State will not be persisted!", this));
			return;
		}
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		m_sId = persistenceManager.GetPersistentId(this);
		persistenceManager.RegisterSaveRoot(this, settings.m_bAutosave);
	}
	
	void ~EL_PersistentScriptedStateBase()
	{
		// Check that we are not in session dtor phase.
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		if(!persistenceManager || (persistenceManager.GetState() == EL_EPersistenceManagerState.SHUTDOWN)) return;
		
		persistenceManager.UnregisterSaveRoot(this);
		persistenceManager.UnloadPersistentId(m_sId);
		
		// Only auto self delete if setting for it is enabled
		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(Type());
		if(m_bDetatched || !settings.m_bSelfDelete) return;
		
		Delete();
	}
}

class EL_ScriptedStateSaveDataBase : EL_DbEntity
{
	int m_iDataLayoutVersion = 1;
	EL_DateTimeUtcAsInt m_iLastSaved;
	
	bool ReadFrom(notnull EL_PersistentScriptedStateBase scriptedState)
	{
		return EL_DbEntityUtils.StructAutoCopy(scriptedState, this);
	}
	
	EL_PersistentScriptedStateBase Spawn()
	{
		return EL_PersistenceManager.GetInstance().SpawnScriptedState(this);
	}
	
	bool ApplyTo(notnull EL_PersistentScriptedStateBase scriptedState)
	{
		return EL_DbEntityUtils.StructAutoCopy(this, scriptedState);
	}
}

class EL_PersistentScriptedStateSettings
{
	protected static ref map<typename, ref EL_PersistentScriptedStateSettings> m_Settings;
    protected static ref map<typename, typename> m_ReverseMapping;
	
	typename m_tSaveDataType;
	bool m_bAutosave;
	bool m_bShutDownSave;
	bool m_bSelfDelete;
	
	void EL_PersistentScriptedStateSettings(typename scriptedStateType, typename saveDataType, bool autoSave = true, bool shutDownSave = true, bool selfDelete = true)
	{
        if(!saveDataType.IsInherited(EL_ScriptedStateSaveDataBase))
        {
            Debug.Error(string.Format("Failed to register '%1' as persistence save struct for '%2'. '%1' must inherit from '%3'.", saveDataType, scriptedStateType, EL_ScriptedStateSaveDataBase));
        }
		
		if(!m_Settings)
		{
			m_Settings = new map<typename, ref EL_PersistentScriptedStateSettings>();
			m_ReverseMapping = new map<typename, typename>();
		}
		
		m_tSaveDataType = saveDataType;
		m_bAutosave = autoSave;
		m_bShutDownSave = shutDownSave;
		m_bSelfDelete = selfDelete;
		
		m_Settings.Set(scriptedStateType, this);
        m_ReverseMapping.Set(saveDataType, scriptedStateType);
	}
	
    static EL_PersistentScriptedStateSettings Get(typename scriptedStateType)
    {
        if(!m_Settings) return null;
        
        return m_Settings.Get(scriptedStateType);
    }
	
	static typename GetScriptedStateType(typename saveDataType)
	{
		if(!m_ReverseMapping) return typename.Empty;
		
		return m_ReverseMapping.Get(saveDataType);
	}
}
