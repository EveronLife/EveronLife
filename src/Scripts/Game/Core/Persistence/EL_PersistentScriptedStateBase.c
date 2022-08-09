class EL_PersistentScriptedStateBase
{
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
		if(!m_sId) return null;
		
		m_iLastSaved = EL_Utils.GetCurrentUtcAsInt();
		
		Tuple3<typename, bool, bool> settings = EL_ScriptedStateSaveDataType.GetSettings(Type());
		
		EL_ScriptedStateSaveDataBase saveData = EL_ScriptedStateSaveDataBase.Cast(settings.param1.Spawn());		
		if(!saveData || !saveData.ReadFrom(this))
		{
			Debug.Error(string.Format("Failed to persist scripted state '%1'. Save-data could not be read.", this));
			return null;
		}
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
		persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
		
		return saveData;
	}
	
	void Delete()
	{
		if(!m_sId) return;
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		
		persistenceManager.UnsubscribeAutoSave(this);
		
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
			return;
		}
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		
		EL_ScriptedStateSaveDataBase saveData = persistenceManager.GetScriptedStateSaveDataBuffer();
		if(saveData)
		{
			// Apply existing save data
			if(!saveData.ApplyTo(this))
			{
				Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", saveData.Type(), saveData.GetId()));
				m_sId = string.Empty;
				return;
			}
			
			m_iLastSaved = saveData.m_iLastSaved;
		}
		else
		{
			// Create from scratch, so we need to generate an id
			m_sId = persistenceManager.GeneratePersistentId();
		}
		
		if(settings.param2)
		{
			persistenceManager.SubscribeAutoSave(this);
		}
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
    
    void EL_ScriptedStateSaveDataType(typename saveDataType, typename scriptedStateType, bool enableAutoSave = true, bool selfDelete = true, string saveDataDbEntityName = "")
    {
        if(!m_Mapping)
		{
			m_Mapping = new map<typename, ref Tuple3<typename, bool, bool>>();
			m_ReverseMapping = new map<typename, typename>();
		}
        
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