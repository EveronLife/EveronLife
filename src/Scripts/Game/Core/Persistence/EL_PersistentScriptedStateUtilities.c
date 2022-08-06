/*
class EL_PersistentScriptedStateSpawner<Class TScriptedState>
{
	static TScriptedState GetSingleton()
	{	
		TScriptedState instance();
		
		EL_PersistentScriptedStateLoader.GetSingleton(instance);
		
		// Return even if GetSingleton returned false, as then its likely to be a first time creation.
		return instance;
	}
}

class EL_PersistentScriptedStateLoader
{
	static bool GetSingleton(Class scriptedState)
	{
		typename saveStructType = EL_PersistentScriptedStateSaveStruct.Get(scriptedState.Type());
		if(!saveStructType) return false;
		
		EL_DbContext context = EL_PersistenceManager.GetInstance().GetDbContext();
		
		array<ref EL_DbEntity> findResults = context.FindAll(saveStructType, limit: 1).GetEntities();
		
		if(!findResults || findResults.Count() != 1) return false;
		
		EL_PersistentScriptedState dbData = EL_PersistentScriptedState.Cast(findResults.Get(0));
		if(!dbData) return false;
		
		return dbData.ApplyTo(scriptedState);
	}
}
*/
