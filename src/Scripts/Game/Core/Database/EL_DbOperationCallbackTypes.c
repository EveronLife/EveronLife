enum EL_EDbOperationStatusCode
{
	SUCCESS,
	
	// System level failure
	FAILURE_CONTEXT_INVALID,
	FAILURE_STORAGE_UNAVAILABLE,
	FAILURE_DATA_MALFORMED,
	
	// User failure
	FAILURE_ID_NOT_FOUND,
	
	// Unknown
	FAILURE_UNKNOWN
}

class EL_DbOperationCallback
{
	protected ref ScriptCallQueue m_Invoker;
	protected Managed m_InvokeInstance;
	protected string m_InvokeMethodName;
	
	protected void ConfigureInvoker(Managed instance, string functionName)
	{
		m_Invoker = new ScriptCallQueue();
		m_InvokeInstance = instance;
		m_InvokeMethodName = functionName;
	}
}

class EL_DbOperationStatusOnlyCallback : EL_DbOperationCallback
{
	static EL_DbOperationStatusOnlyCallback FromMethod(Managed instance, string functionName)
	{
		EL_DbOperationStatusOnlyCallback callback();
		callback.ConfigureInvoker(instance, functionName);
		return callback;
	}
	
	void _SetCompleted(EL_EDbOperationStatusCode code)
	{
		if(code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess();
		}
		else
		{
			OnFailure(code);
		}
		
		if(m_Invoker && m_InvokeInstance && m_InvokeMethodName)
		{
			m_Invoker.CallByName(m_InvokeInstance, m_InvokeMethodName, code);
			m_Invoker.Tick(1);
		}
	}
	
	void OnSuccess();
	
	void OnFailure(EL_EDbOperationStatusCode resultCode);
}

class EL_DbFindCallbackBase : EL_DbOperationCallback
{
	void _SetCompleted(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults);
}

class EL_DbFindCallback<Class TEntityType> : EL_DbFindCallbackBase
{
	static EL_DbFindCallback<TEntityType> FromMethod(Managed instance, string functionName)
	{
		EL_DbFindCallback<TEntityType> callback();
		callback.ConfigureInvoker(instance, functionName);
		return callback;
	}
	
	override void _SetCompleted(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		array<ref TEntityType> strongTypedResults = EL_RefArrayCaster<EL_DbEntity, TEntityType>.Convert(findResults);
		
		if(code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(strongTypedResults);
		}
		else
		{
			OnFailure(code);
		}

		if(m_Invoker && m_InvokeInstance && m_InvokeMethodName)
		{
			m_Invoker.CallByName(m_InvokeInstance, m_InvokeMethodName, code, strongTypedResults);
			m_Invoker.Tick(1);
		}
	}
	
	void OnSuccess(array<ref TEntityType> resultData);
	
	void OnFailure(EL_EDbOperationStatusCode resultCode);
}

class EL_DbFindCallbackSingle<Class TEntityType> : EL_DbFindCallbackBase
{
	static EL_DbFindCallbackSingle<TEntityType> FromMethod(Managed instance, string functionName)
	{
		EL_DbFindCallbackSingle<TEntityType> callback();
		callback.ConfigureInvoker(instance, functionName);
		return callback;
	}
	
	override void _SetCompleted(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		TEntityType typedResult;
		
		if(findResults.Count() > 0)
		{
			typedResult = TEntityType.Cast(findResults.Get(0));
		}
		
		if(code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(typedResult);
		}
		else
		{
			OnFailure(code);
		}

		if(m_Invoker && m_InvokeInstance && m_InvokeMethodName)
		{
			m_Invoker.CallByName(m_InvokeInstance, m_InvokeMethodName, code, typedResult);
			m_Invoker.Tick(1);
		}
	}
	
	void OnSuccess(TEntityType resultData);
	
	void OnFailure(EL_EDbOperationStatusCode resultCode);
}
