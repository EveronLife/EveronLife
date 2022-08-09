class EL_PersistentScriptedStateLoader<Class TScriptedState>
{
	static TScriptedState GetSingleton()
	{	
		typename saveDataType;
		if(!TypeAndSettingsValidation(saveDataType)) return null;
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
	    array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, limit: 1).GetEntities();
	    if(!findResults || findResults.Count() != 1) return null;
	    
	    return TScriptedState.Cast(persistenceManager.SpawnScriptedState(EL_ScriptedStateSaveDataBase.Cast(findResults.Get(0))));
	}
	
	static TScriptedState Get(string persistentId)
	{
		typename saveDataType;
		if(!TypeAndSettingsValidation(saveDataType)) return null;
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
	    array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, EL_DbFind.Id().Equals(persistentId), limit: 1).GetEntities();
	    if(!findResults || findResults.Count() != 1) return null;
	    
	    return TScriptedState.Cast(persistenceManager.SpawnScriptedState(EL_ScriptedStateSaveDataBase.Cast(findResults.Get(0))));
	}
	
	static array<ref TScriptedState> Get(array<string> persistentIds)
	{
		typename saveDataType;
		if(!TypeAndSettingsValidation(saveDataType)) return null;
		
		array<ref TScriptedState> resultStates();
	    
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
	    array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, EL_DbFind.Id().EqualsAnyOf(persistentIds)).GetEntities();
	    if(!findResults) return resultStates;
	    
	    foreach(EL_DbEntity findResult : findResults)
	    {
	        TScriptedState state = TScriptedState.Cast(persistenceManager.SpawnScriptedState(EL_ScriptedStateSaveDataBase.Cast(findResult)));
	        
	        if(state) resultStates.Insert(state);			
	    }
	    
	    return resultStates;
	}
	
	protected static bool TypeAndSettingsValidation(out typename saveDataType)
	{
		typename resultType = TScriptedState;
		if(!resultType.IsInherited(EL_PersistentScriptedStateBase)) return false;

	    EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(TScriptedState);
		if(!settings || !settings.m_tSaveDataType)
		{
			Debug.Error(string.Format("Scripted state type '%1' needs to have no save-data configured to be loaded!", TScriptedState));
			return false;
		}
		
		saveDataType = settings.m_tSaveDataType;
		
		return true;
	}
}
