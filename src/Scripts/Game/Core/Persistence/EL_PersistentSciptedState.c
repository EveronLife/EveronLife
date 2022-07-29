class EL_PersistentSciptedState : EL_DbEntity
{
	bool ReadFrom(Class scriptedState);
	
	bool ApplyTo(Class scriptedState);
}

class EL_PersistentSciptedStateStruct
{
	protected static ref map<typename, typename> m_Mapping;
	
	void EL_PersistentSciptedStateStruct(typename saveStructType, typename scriptedStateType)
	{
		if(!m_Mapping) m_Mapping = new map<typename, typename>();
		
		if(!saveStructType.IsInherited(EL_PersistentSciptedState))
		{
			Debug.Error(string.Format("Failed to register '%1' as persistence save struct for '%2'. '%1' must inherit from '%3'.", saveStructType, scriptedStateType, EL_PersistentSciptedState));
		}
		
		m_Mapping.Set(saveStructType, scriptedStateType);
	}
	
	static typename Get(typename scriptedStateType)
	{
		if(!m_Mapping) return typename.Empty;
		
		return m_Mapping.Get(scriptedStateType);
	}
}

class EL_PersistentSciptedStateSpawner<Class TScriptedState>
{
	static TScriptedState GetSingleton(string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useDbContextCache = true)
	{	
		TScriptedState instance();
		
		EL_PersistentSciptedStateLoader.GetSingleton(instance, dataSource, useDbContextCache);
		
		// Return blank even if GetSingleton returned false, as then its likely to be a first time creation.
		return instance;
	}
}

class EL_PersistentSciptedStateLoader
{
	static bool GetSingleton(Class scriptedState, string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useDbContextCache = true)
	{
		typename saveStructType = EL_PersistentSciptedStateStruct.Get(scriptedState.Type());
		if(!saveStructType) return null;
		
		EL_DbEntityRepositoryBase repository = EL_DbEntityRepositoryFactory.GetRepository(EL_DbEntityRepositoryType.Get(saveStructType), dataSource, useDbContextCache);
		
		array<ref EL_DbEntity> findResults = repository.GetDbContext().FindAll(saveStructType, limit: 1);
		
		if(findResults.Count() != 1) return null;
		
		EL_PersistentSciptedState dbData = EL_PersistentSciptedState.Cast(findResults.Get(0));
		if(!dbData) return null;
		
		return dbData.ApplyTo(scriptedState);
	}
}
