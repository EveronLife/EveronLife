enum EL_DbOperationStatusCode
{
	SUCCESS,
	
	// System level failure
	FAILURE_CONTEXT_INVALID,
	
	// User failure
	FAILURE_ID_NOT_FOUND,
	FAILURE_REQUEST_INVALID,
	
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
	
	void _SetCompleted(EL_DbOperationStatusCode code)
	{
		if(code == EL_DbOperationStatusCode.SUCCESS)
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
	
	void OnFailure(EL_DbOperationStatusCode resultCode);
}

class EL_DbFindCallbackBase : EL_DbOperationCallback
{
	void _SetCompleted(EL_DbOperationStatusCode code, array<ref EL_DbEntity> findResults);
}

class EL_DbFindCallback<Class TDbEntityType> : EL_DbFindCallbackBase
{
	static EL_DbFindCallback<TDbEntityType> FromMethod(Managed instance, string functionName)
	{
		EL_DbFindCallback<TDbEntityType> callback();
		callback.ConfigureInvoker(instance, functionName);
		return callback;
	}
	
	override void _SetCompleted(EL_DbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		array<ref TDbEntityType> strongTypedResults();
		
		foreach(EL_DbEntity findResult : findResults)
		{
			TDbEntityType typedResult = TDbEntityType.Cast(findResult);
			
			if(typedResult) strongTypedResults.Insert(typedResult);
		}
		
		if(code == EL_DbOperationStatusCode.SUCCESS)
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
	
	void OnSuccess(array<ref TDbEntityType> resultData);
	
	void OnFailure(EL_DbOperationStatusCode resultCode);
}

class EL_DbFindCallbackSingle<Class TDbEntityType> : EL_DbFindCallbackBase
{
	static EL_DbFindCallbackSingle<TDbEntityType> FromMethod(Managed instance, string functionName)
	{
		EL_DbFindCallbackSingle<TDbEntityType> callback();
		callback.ConfigureInvoker(instance, functionName);
		return callback;
	}
	
	override void _SetCompleted(EL_DbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		TDbEntityType typedResult;
		
		if(findResults.Count() > 0)
		{
			typedResult = TDbEntityType.Cast(findResults.Get(0));
		}
		
		if(code == EL_DbOperationStatusCode.SUCCESS)
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
	
	void OnSuccess(TDbEntityType resultData);
	
	void OnFailure(EL_DbOperationStatusCode resultCode);
}
