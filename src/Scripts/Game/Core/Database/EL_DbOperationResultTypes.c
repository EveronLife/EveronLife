enum EL_EDbOperationStatusCode
{
	SUCCESS,

	// System level failure
	FAILURE_CONTEXT_INVALID,
	FAILURE_STORAGE_UNAVAILABLE,
	FAILURE_DATA_MALFORMED,

	// User failure
	FAILURE_ID_NOT_SET,
	FAILURE_ID_NOT_FOUND,

	// Unknown
	FAILURE_UNKNOWN
}

class EL_DbFindResultBase
{
	protected EL_EDbOperationStatusCode m_eStatusCode;

	//------------------------------------------------------------------------------------------------
	EL_EDbOperationStatusCode GetStatusCode()
	{
		return m_eStatusCode;
	}

	//------------------------------------------------------------------------------------------------
	bool Success()
	{
		return m_eStatusCode == EL_EDbOperationStatusCode.SUCCESS;
	}
}

class EL_DbFindResults<Class TEntityType> : EL_DbFindResultBase
{
	protected ref array<ref TEntityType> m_aEntities;

	//------------------------------------------------------------------------------------------------
	array<ref TEntityType> GetEntities()
	{
		return m_aEntities;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindResults(EL_EDbOperationStatusCode statusCode, array<ref TEntityType> entities = null)
	{
		m_eStatusCode = statusCode;
		m_aEntities = entities;
	}
}

class EL_DbFindResult<Class TEntityType> : EL_DbFindResultBase
{
	protected ref TEntityType m_pEntity;

	//------------------------------------------------------------------------------------------------
	TEntityType GetEntity()
	{
		return m_pEntity;
	}

	//------------------------------------------------------------------------------------------------
	TEntityType GetSuccessEntity()
	{
		if (m_eStatusCode != EL_EDbOperationStatusCode.SUCCESS) return null;

		return m_pEntity;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindResults(EL_EDbOperationStatusCode statusCode, TEntityType entity = null)
	{
		m_eStatusCode = statusCode;
		m_pEntity = entity;
	}
}

class EL_DbOperationCallback : EL_Callback
{
}

class EL_DbOperationStatusOnlyCallback : EL_DbOperationCallback
{
	//------------------------------------------------------------------------------------------------
	void Invoke(EL_EDbOperationStatusCode code)
	{
		if (m_pInvokeInstance &&
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, code)) return;

		if (code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(m_pContext);
		}
		else
		{
			OnFailure(m_pContext, code);
		}
	}

	//------------------------------------------------------------------------------------------------
	void OnSuccess(Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode);
}

class EL_DbFindCallbackBase : EL_DbOperationCallback
{
	void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults);
}

class EL_DbFindCallback<Class TEntityType> : EL_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	override void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		array<ref TEntityType> strongTypedResults = EL_RefArrayCaster<EL_DbEntity, TEntityType>.Convert(findResults);

		if (m_pInvokeInstance &&
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, code, strongTypedResults)) return;

		if (code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(m_pContext, strongTypedResults);
		}
		else
		{
			OnFailure(m_pContext, code);
		}
	}

	//------------------------------------------------------------------------------------------------
	void OnSuccess(Managed context, array<ref TEntityType> resultData);

	//------------------------------------------------------------------------------------------------
	void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode);
}

class EL_DbFindCallbackSingle<Class TEntityType> : EL_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	override void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		TEntityType typedResult;

		if (findResults.Count() > 0)
		{
			typedResult = TEntityType.Cast(findResults.Get(0));
		}

		if (m_pInvokeInstance &&
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, code, typedResult)) return;

		if (code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(m_pContext, typedResult);
		}
		else
		{
			OnFailure(m_pContext, code);
		}
	}

	//------------------------------------------------------------------------------------------------
	void OnSuccess(Managed context, TEntityType resultData);

	//------------------------------------------------------------------------------------------------
	void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode);
}

class EL_DbFindCallbackSingleton<Class TEntityType> : EL_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	override void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		TEntityType typedResult;

		if (findResults.Count() > 0)
		{
			typedResult = TEntityType.Cast(findResults.Get(0));
		}
		else
		{
			typename spawnType = TEntityType;
			typedResult = TEntityType.Cast(spawnType.Spawn());
		}

		if (m_pInvokeInstance &&
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, code, typedResult)) return;

		if (code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(m_pContext, typedResult);
		}
		else
		{
			OnFailure(m_pContext, code);
		}
	}

	//------------------------------------------------------------------------------------------------
	void OnSuccess(Managed context, TEntityType resultData);

	//------------------------------------------------------------------------------------------------
	void OnFailure(Managed context, EL_EDbOperationStatusCode resultCode);
}
