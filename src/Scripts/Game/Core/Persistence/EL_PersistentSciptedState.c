/*
class EL_PersistentScriptedState : EL_DbEntity
{
	bool ReadFrom(Class scriptedState);
	bool ApplyTo(Class scriptedState);
}

class EL_PersistentScriptedStateSaveStruct
{
	protected static ref map<typename, typename> m_Mapping;
	
	void EL_PersistentScriptedStateSaveStruct(typename saveStructType, typename scriptedStateType, string dbEntityName = "")
	{
		if(!m_Mapping) m_Mapping = new map<typename, typename>();
		
		if(!saveStructType.IsInherited(EL_PersistentScriptedState))
		{
			Debug.Error(string.Format("Failed to register '%1' as persistence save struct for '%2'. '%1' must inherit from '%3'.", saveStructType, scriptedStateType, EL_PersistentScriptedState));
		}
		
		m_Mapping.Set(scriptedStateType, saveStructType);
		
		if(dbEntityName)
		{
			EL_DbName.Set(saveStructType, dbEntityName);
		}
	}
	
	static typename Get(typename scriptedStateType)
	{
		if(!m_Mapping) return typename.Empty;
		
		return m_Mapping.Get(scriptedStateType);
	}
}
*/