class EL_PersistentScriptedStateBase
{
	[NonSerialized()]
	protected bool m_bActive;
	
	protected string m_sId;
	protected DateTimeUtcAsInt m_iLastSaved;
	
	string GetPersistentId()
	{
		return m_sId;
	}
	
	DateTimeUtcAsInt GetLastSaved()
	{
		return m_iLastSaved;
	}
	
	EL_ScriptedStateSaveDataBase Save()
	{
		if(!m_bActive) return null;
		
		m_iLastSaved = EL_Utils.GetCurrentUtcAsInt();
		
		Tuple3<typename, bool, bool> settings = EL_ScriptedStateSaveDataType.GetSettings(Type());
		
		EL_ScriptedStateSaveDataBase saveData = EL_ScriptedStateSaveDataBase.Cast(settings.param1.Spawn());		
		if(!saveData || !saveData.ReadFrom(this))
		{
			Debug.Error(string.Format("Failed to persist scripted state '%1'. Save-data could not be read.", this));
			return null;
		}
		
		return saveData;
	}
	
	void Delete()
	{
		if(!m_bActive) return;
		
		// Once deleted this scripted state becomes inactive in terms of persistence
		m_bActive = false;
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
		GetGame().GetScriptModule().Call(persistenceManager, "UnsubscribeAutoSave", false, null, this);
		
		Tuple3<typename, bool, bool> settings = EL_ScriptedStateSaveDataType.GetSettings(Type());
		if(!settings || !settings.param3) return;
		
		persistenceManager.GetDbContext().RemoveAsync(settings.param1, m_sId);
		
		m_sId = string.Empty;
		m_iLastSaved = 0;
	}
	
	void EL_PersistentScriptedStateBase()
	{
		Tuple3<typename, bool, bool> settings = EL_ScriptedStateSaveDataType.GetSettings(Type());
		if(!settings) return;
		
		if(!settings.param1 || settings.param1 == EL_ScriptedStateSaveDataBase)
		{
			Debug.Error(string.Format("Missing or invalid save-data type on persistend scripted state '%1'. State will not be persisted!", this));
		}
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
		GetGame().GetScriptModule().Call(persistenceManager, "GeneratePersistentId", false, m_sId, this);
		if(!m_sId) return;
		
		if(settings.param2)
		{
			GetGame().GetScriptModule().Call(persistenceManager, "SubscribeAutoSave", false, null, this);
		}
		
		m_bActive = true;
	}
	
	void ~EL_PersistentScriptedStateBase()
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if(!persistenceManager || !persistenceManager.IsActive()) return;
		
		Delete();
	}
}

class EL_ScriptedStateSaveDataBase : EL_DbEntity
{
	DateTimeUtcAsInt m_iLastSaved;
	
	bool ReadFrom(EL_PersistentScriptedStateBase scriptedState)
	{
		return EL_DbEntityUtils.StructAutoCopy(scriptedState, this);
	}
	
	EL_PersistentScriptedStateBase Spawn()
	{
		return EL_PersistenceManager.GetInstance().SpawnScriptedState(this);
	}
	
	bool ApplyTo(EL_PersistentScriptedStateBase scriptedState)
	{
		return EL_DbEntityUtils.StructAutoCopy(this, scriptedState);
	}
}

class EL_ScriptedStateSaveDataType
{
    protected static ref map<typename, ref Tuple3<typename, bool, bool>> m_Mapping;
    protected static ref map<typename, typename> m_ReverseMapping;
    
    void EL_ScriptedStateSaveDataType(typename scriptedStateType, typename saveDataType, bool enableAutoSave = true, bool selfDelete = true, string saveDataDbEntityName = "")
    {
        if(!m_Mapping) m_Mapping = new map<typename, ref Tuple3<typename, bool, bool>>();
        
        if(!saveDataType.IsInherited(EL_ScriptedStateSaveDataBase))
        {
            Debug.Error(string.Format("Failed to register '%1' as persistence save struct for '%2'. '%1' must inherit from '%3'.", saveDataType, scriptedStateType, EL_ScriptedStateSaveDataBase));
        }
        
        m_Mapping.Set(scriptedStateType, new Tuple3<typename, bool, bool>(saveDataType, enableAutoSave, selfDelete));
        m_ReverseMapping.Set(saveDataType, scriptedStateType);
        
        if(saveDataDbEntityName)
        {
            EL_DbName.Set(saveDataType, saveDataDbEntityName);
        }
    }
    
    static Tuple3<typename, bool, bool> GetSettings(typename scriptedStateType)
    {
        if(!m_Mapping) return null;
        
        return m_Mapping.Get(scriptedStateType);
    }
	
	static typename GetScriptedStateType(typename saveDataType)
	{
		if(!m_ReverseMapping) return typename.Empty;
		
		return m_ReverseMapping.Get(saveDataType);
	}
}

// TODO: Proxy variant with proxy save-data?