class EL_PersistentScriptedStateLoader<Class TScriptedState>
{
	static TScriptedState GetSingleton()
	{	
		typename saveDataType;
		if(!TypeAndSettingsValidation(saveDataType)) return null;
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
	    array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, limit: 1).GetEntities();
	    if(!findResults || findResults.IsEmpty())
		{
			typename spawnType = TScriptedState;
			return TScriptedState.Cast(spawnType.Spawn());
		}
	    
	    return TScriptedState.Cast(persistenceManager.SpawnScriptedState(EL_ScriptedStateSaveDataBase.Cast(findResults.Get(0))));
	}
	
	static void GetSingletonAsync(notnull EL_PersistenceLoaderCallbackSingle<TScriptedState> callback)
	{
		typename saveDataType;
		if(!TypeAndSettingsValidation(saveDataType)) return;
		
		EL_ScriptStateLoaderCallbackInvokerSingle<TScriptedState> callbackInvoker(callback);
		EL_ScriptStateLoaderCallbackSingle processorCallback();
		processorCallback.Setup(callbackInvoker, true, TScriptedState);
	    EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, limit: 1, callback: processorCallback);
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
	
	static void GetAsync(string persistentId, notnull EL_PersistenceLoaderCallbackSingle<TScriptedState> callback)
	{
		typename saveDataType;
		if(!TypeAndSettingsValidation(saveDataType)) return;
		
		EL_ScriptStateLoaderCallbackInvokerSingle<TScriptedState> callbackInvoker(callback);
		EL_ScriptStateLoaderCallbackSingle processorCallback();
		processorCallback.Setup(callbackInvoker, false, TScriptedState);
	    EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, EL_DbFind.Id().Equals(persistentId), limit: 1, callback: processorCallback);
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
	
	static void GetAsync(array<string> persistentIds, notnull EL_PersistenceLoaderCallbackMultiple<TScriptedState> callback)
	{
		typename saveDataType;
		if(!TypeAndSettingsValidation(saveDataType)) return;
		
		EL_ScriptStateLoaderCallbackInvokerMultiple<TScriptedState> callbackInvoker(callback);
		EL_ScriptStateLoaderCallbackMultiple processorCallback(callbackInvoker);
		processorCallback.Setup(callbackInvoker);
		EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, EL_DbFind.Id().EqualsAnyOf(persistentIds), callback: processorCallback);
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

class EL_ScriptStateLoaderCallbackSingle : EL_DbFindCallbackSingle<EL_ScriptedStateSaveDataBase>
{
	ref EL_ScriptStateLoaderCallbackInvokerBase m_pCallbackInvoker;
	bool m_bCreateSingleton;
	typename m_tCreateType;
	
	override void OnSuccess(Managed context, EL_ScriptedStateSaveDataBase resultData)
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
		EL_PersistentScriptedStateBase resultScriptedState;
		
		if (resultData)
		{
			resultScriptedState = EL_PersistentScriptedStateBase.Cast(persistenceManager.SpawnScriptedState(resultData))
		}
		else if(m_bCreateSingleton)
		{
			resultScriptedState = EL_PersistentScriptedStateBase.Cast(m_tCreateType.Spawn());
		}
		
		GetGame().GetScriptModule().Call(m_pCallbackInvoker, "Invoke", false, null, resultScriptedState);
	}
	
	override void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode)
	{
		GetGame().GetScriptModule().Call(m_pCallbackInvoker, "Invoke", false, null, null);
	}
	
	void Setup(EL_ScriptStateLoaderCallbackInvokerBase callbackInvoker, bool createSingleton, typename createType)
	{
		m_pCallbackInvoker = callbackInvoker;
		m_bCreateSingleton = createSingleton;
		m_tCreateType = createType;
	}
}

class EL_ScriptStateLoaderCallbackMultiple : EL_DbFindCallback<EL_ScriptedStateSaveDataBase>
{
	ref EL_ScriptStateLoaderCallbackInvokerBase m_pCallbackInvoker;
	
	override void OnSuccess(Managed context, array<ref EL_ScriptedStateSaveDataBase> resultData)
	{
		if (resultData)
		{
			EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
			
			foreach (EL_ScriptedStateSaveDataBase saveData : resultData)
			{
				EL_PersistentScriptedStateBase resultScriptedState = EL_PersistentScriptedStateBase.Cast(persistenceManager.SpawnScriptedState(saveData));
				if (resultScriptedState) GetGame().GetScriptModule().Call(m_pCallbackInvoker, "AddResult", false, null, resultScriptedState);
			}
		}
		
		GetGame().GetScriptModule().Call(m_pCallbackInvoker, "Invoke", false, null);
	}
	
	override void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode)
	{
		GetGame().GetScriptModule().Call(m_pCallbackInvoker, "Invoke", false, null);
	}
	
	void Setup(EL_ScriptStateLoaderCallbackInvokerBase callbackInvoker)
	{
		m_pCallbackInvoker = callbackInvoker;
	}
}

// Seperate invoker because we can't strong type the EL_ScriptStateLoaderCallback via template directly until https://feedback.bistudio.com/T167295 is fixed.
class EL_ScriptStateLoaderCallbackInvokerBase 
{
	ref EL_PersistenceLoaderCallback m_pCallback;
	
	void EL_ScriptStateLoaderCallbackInvokerBase(EL_PersistenceLoaderCallback callback)
	{
		m_pCallback = callback;
	}
}

class EL_ScriptStateLoaderCallbackInvokerSingle<Class T> : EL_ScriptStateLoaderCallbackInvokerBase
{
	void Invoke(EL_PersistentScriptedStateBase resultScriptedState)
	{
		T typedResult = T.Cast(resultScriptedState);
		auto typedCallback = EL_PersistenceLoaderCallbackSingle<T>.Cast(m_pCallback);
		if (typedCallback) typedCallback.Invoke(typedResult);
	}
}

class EL_ScriptStateLoaderCallbackInvokerMultiple<Class T> : EL_ScriptStateLoaderCallbackInvokerBase
{
	ref array<ref T> m_aResultBuffer = {};
	
	void AddResult(EL_PersistentScriptedStateBase resultScriptedState)
	{
		T resultTyped = T.Cast(resultScriptedState);
		if (resultTyped) m_aResultBuffer.Insert(resultTyped);
	}
	
	void Invoke()
	{
		auto typedCallback = EL_PersistenceLoaderCallbackMultiple<T>.Cast(m_pCallback);
		if (typedCallback) typedCallback.Invoke(m_aResultBuffer);
	}
}
