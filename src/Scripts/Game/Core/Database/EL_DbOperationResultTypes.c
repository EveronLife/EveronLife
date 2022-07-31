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

class EL_DbFindResultBase
{
	protected EL_EDbOperationStatusCode m_eStatusCode;
	
	EL_EDbOperationStatusCode GetStatusCode()
	{
		return m_eStatusCode;
	}
	
	bool Success()
	{
		return m_eStatusCode == EL_EDbOperationStatusCode.SUCCESS;
	}
}

class EL_DbFindResults<Class TEntityType> : EL_DbFindResultBase
{
	protected ref array<ref TEntityType> m_aEntities;
	
	array<ref TEntityType> GetEntities()
	{
		return m_aEntities;
	}
	
	void EL_DbFindResults(EL_EDbOperationStatusCode statusCode, array<ref TEntityType> entities)
	{
		m_eStatusCode = statusCode;
		m_aEntities = entities;
	}
}

class EL_DbFindResult<Class TEntityType> : EL_DbFindResultBase
{
	protected ref TEntityType m_pEntity;
	
	TEntityType GetEntity()
	{
		return m_pEntity;
	}
	
	void EL_DbFindResults(EL_EDbOperationStatusCode statusCode, TEntityType entity)
	{
		m_eStatusCode = statusCode;
		m_pEntity = entity;
	}
}

class EL_DbOperationCallback
{
	protected Class m_InvokeInstance;
	protected string m_InvokeMethodName;
	
	protected void ConfigureInvoker(Class instance, string functionName)
	{
		m_InvokeInstance = instance;
		m_InvokeMethodName = functionName;
	}
}

class EL_DbOperationStatusOnlyCallback : EL_DbOperationCallback
{
	static EL_DbOperationStatusOnlyCallback FromMethod(Class instance, string functionName)
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
		
		if(m_InvokeInstance && m_InvokeMethodName)
		{
			GetGame().GetScriptModule().Call(m_InvokeInstance, m_InvokeMethodName, true, null, code);
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

		if(m_InvokeInstance && m_InvokeMethodName)
		{
			GetGame().GetScriptModule().Call(m_InvokeInstance, m_InvokeMethodName, true, null, code, strongTypedResults);
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

		if(m_InvokeInstance && m_InvokeMethodName)
		{
			GetGame().GetScriptModule().Call(m_InvokeInstance, m_InvokeMethodName, true, null, code, typedResult);
		}
	}
	
	void OnSuccess(TEntityType resultData);
	
	void OnFailure(EL_EDbOperationStatusCode resultCode);
}
