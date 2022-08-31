class EL_PersistentScriptedStateLoader<Class TScriptedState>
{
	//------------------------------------------------------------------------------------------------
	//! Load and instantiate or create the scripted state singleton
	//! \return scripted state instance
	static TScriptedState LoadSingleton()
	{
		typename saveDataType;
		if (!TypeAndSettingsValidation(saveDataType)) return null;

		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, limit: 1).GetEntities();
		if (!findResults || findResults.IsEmpty())
		{
			typename spawnType = TScriptedState;
			return TScriptedState.Cast(spawnType.Spawn());
		}

		return TScriptedState.Cast(persistenceManager.SpawnScriptedState(EL_ScriptedStateSaveDataBase.Cast(findResults.Get(0))));
	}

	//------------------------------------------------------------------------------------------------
	//! s. LoadSingleton()
	static void LoadSingletonAsync(notnull EL_ScriptedStateLoaderCallbackSingle<TScriptedState> callback)
	{
		typename saveDataType;
		if (!TypeAndSettingsValidation(saveDataType)) return;

		EL_ScriptedStateLoaderCallbackInvokerSingle<TScriptedState> callbackInvoker(callback);
		EL_ScriptedStateLoaderProcessorCallbackSingle processorCallback();
		processorCallback.Setup(callbackInvoker, true, TScriptedState);
		EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, limit: 1, callback: processorCallback);
	}

	//------------------------------------------------------------------------------------------------
	//! Load and instantiate a scripted state by persistent id
	//! \param persistentId Persistent id
	//! \return instantiated scripted state instance or null on failure
	static TScriptedState Load(string persistentId)
	{
		typename saveDataType;
		if (!TypeAndSettingsValidation(saveDataType)) return null;

		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, EL_DbFind.Id().Equals(persistentId), limit: 1).GetEntities();
		if (!findResults || findResults.Count() != 1) return null;

		return TScriptedState.Cast(persistenceManager.SpawnScriptedState(EL_ScriptedStateSaveDataBase.Cast(findResults.Get(0))));
	}

	//------------------------------------------------------------------------------------------------
	//! s. Load()
	static void LoadAsync(string persistentId, notnull EL_ScriptedStateLoaderCallbackSingle<TScriptedState> callback)
	{
		typename saveDataType;
		if (!TypeAndSettingsValidation(saveDataType)) return;

		EL_ScriptedStateLoaderCallbackInvokerSingle<TScriptedState> callbackInvoker(callback);
		EL_ScriptedStateLoaderProcessorCallbackSingle processorCallback();
		processorCallback.Setup(callbackInvoker, false, TScriptedState);
		EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, EL_DbFind.Id().Equals(persistentId), limit: 1, callback: processorCallback);
	}

	//------------------------------------------------------------------------------------------------
	//! Load and instantiate multiple scripted states by their persistent id
	//! \param persistentIds Persistent ids
	//! \return array of scripted state instances or emtpy
	static array<ref TScriptedState> Load(array<string> persistentIds)
	{
		typename saveDataType;
		if (!TypeAndSettingsValidation(saveDataType)) return null;

		array<ref TScriptedState> resultStates();

		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, EL_DbFind.Id().EqualsAnyOf(persistentIds)).GetEntities();
		if (findResults)
		{
			foreach (EL_DbEntity findResult : findResults)
			{
				TScriptedState state = TScriptedState.Cast(persistenceManager.SpawnScriptedState(EL_ScriptedStateSaveDataBase.Cast(findResult)));
				if (state) resultStates.Insert(state);
			}
		}

		return resultStates;
	}

	//------------------------------------------------------------------------------------------------
	//! s. Load(array<string>)
	static void LoadAsync(array<string> persistentIds, notnull EL_ScriptedStateLoaderCallbackMultiple<TScriptedState> callback)
	{
		typename saveDataType;
		if (!TypeAndSettingsValidation(saveDataType)) return;

		EL_ScriptedStateLoaderCallbackInvokerMultiple<TScriptedState> callbackInvoker(callback);
		EL_ScriptedStateLoaderProcessorCallbackMultiple processorCallback(callbackInvoker);
		processorCallback.Setup(callbackInvoker);
		EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, EL_DbFind.Id().EqualsAnyOf(persistentIds), callback: processorCallback);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool TypeAndSettingsValidation(out typename saveDataType)
	{
		typename resultType = TScriptedState;
		if (!resultType.IsInherited(EL_PersistentScriptedStateBase)) return false;

		EL_PersistentScriptedStateSettings settings = EL_PersistentScriptedStateSettings.Get(TScriptedState);
		if (!settings || !settings.m_tSaveDataType)
		{
			Debug.Error(string.Format("Scripted state type '%1' needs to have no save-data configured to be loaded!", TScriptedState));
			return false;
		}

		saveDataType = settings.m_tSaveDataType;

		return true;
	}
}

class EL_ScriptedStateLoaderCallback : EL_Callback
{
}

class EL_ScriptedStateLoaderCallbackSingle<Class T> : EL_ScriptedStateLoaderCallback
{
	//------------------------------------------------------------------------------------------------
	void Invoke(T data)
	{
		if (m_pInvokeInstance &&
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, data)) return;

		OnComplete(m_pContext, data);
	}

	//------------------------------------------------------------------------------------------------
	void OnComplete(Managed context, T data);
}

class EL_ScriptedStateLoaderCallbackMultiple<Class T> : EL_ScriptedStateLoaderCallback
{
	//------------------------------------------------------------------------------------------------
	void Invoke(array<ref T> data)
	{
		if (m_pInvokeInstance &&
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, data)) return;

		OnComplete(m_pContext, data);
	}

	//------------------------------------------------------------------------------------------------
	void OnComplete(Managed context, array<ref T> data);
}

class EL_ScriptedStateLoaderProcessorCallbackSingle : EL_DbFindCallbackSingle<EL_ScriptedStateSaveDataBase>
{
	ref EL_ScriptedStateLoaderCallbackInvokerBase m_pCallbackInvoker;
	bool m_bCreateSingleton;
	typename m_tCreateType;

	//------------------------------------------------------------------------------------------------
	override void OnSuccess(Managed context, EL_ScriptedStateSaveDataBase resultData)
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();

		EL_PersistentScriptedStateBase resultScriptedState;

		if (resultData)
		{
			resultScriptedState = EL_PersistentScriptedStateBase.Cast(persistenceManager.SpawnScriptedState(resultData))
		}
		else if (m_bCreateSingleton)
		{
			resultScriptedState = EL_PersistentScriptedStateBase.Cast(m_tCreateType.Spawn());
		}

		GetGame().GetScriptModule().Call(m_pCallbackInvoker, "Invoke", false, null, resultScriptedState);
	}

	//------------------------------------------------------------------------------------------------
	override void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode)
	{
		GetGame().GetScriptModule().Call(m_pCallbackInvoker, "Invoke", false, null, null);
	}

	//------------------------------------------------------------------------------------------------
	void Setup(EL_ScriptedStateLoaderCallbackInvokerBase callbackInvoker, bool createSingleton, typename createType)
	{
		m_pCallbackInvoker = callbackInvoker;
		m_bCreateSingleton = createSingleton;
		m_tCreateType = createType;
	}
}

class EL_ScriptedStateLoaderProcessorCallbackMultiple : EL_DbFindCallback<EL_ScriptedStateSaveDataBase>
{
	ref EL_ScriptedStateLoaderCallbackInvokerBase m_pCallbackInvoker;

	//------------------------------------------------------------------------------------------------
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

	//------------------------------------------------------------------------------------------------
	override void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode)
	{
		GetGame().GetScriptModule().Call(m_pCallbackInvoker, "Invoke", false, null);
	}

	//------------------------------------------------------------------------------------------------
	void Setup(EL_ScriptedStateLoaderCallbackInvokerBase callbackInvoker)
	{
		m_pCallbackInvoker = callbackInvoker;
	}
}

// Seperate invoker because we can't strong type the EL_ScriptedStateLoaderCallback via template directly until https://feedback.bistudio.com/T167295 is fixed.
class EL_ScriptedStateLoaderCallbackInvokerBase
{
	ref EL_ScriptedStateLoaderCallback m_pCallback;

	//------------------------------------------------------------------------------------------------
	void EL_ScriptedStateLoaderCallbackInvokerBase(EL_ScriptedStateLoaderCallback callback)
	{
		m_pCallback = callback;
	}
}

class EL_ScriptedStateLoaderCallbackInvokerSingle<Class T> : EL_ScriptedStateLoaderCallbackInvokerBase
{
	//------------------------------------------------------------------------------------------------
	void Invoke(EL_PersistentScriptedStateBase resultScriptedState)
	{
		T typedResult = T.Cast(resultScriptedState);
		auto typedCallback = EL_ScriptedStateLoaderCallbackSingle<T>.Cast(m_pCallback);
		if (typedCallback) typedCallback.Invoke(typedResult);
	}
}

class EL_ScriptedStateLoaderCallbackInvokerMultiple<Class T> : EL_ScriptedStateLoaderCallbackInvokerBase
{
	ref array<ref T> m_aResultBuffer = {};

	//------------------------------------------------------------------------------------------------
	void AddResult(EL_PersistentScriptedStateBase resultScriptedState)
	{
		T resultTyped = T.Cast(resultScriptedState);
		if (resultTyped) m_aResultBuffer.Insert(resultTyped);
	}

	//------------------------------------------------------------------------------------------------
	void Invoke()
	{
		auto typedCallback = EL_ScriptedStateLoaderCallbackMultiple<T>.Cast(m_pCallback);
		if (typedCallback) typedCallback.Invoke(m_aResultBuffer);
	}
}
