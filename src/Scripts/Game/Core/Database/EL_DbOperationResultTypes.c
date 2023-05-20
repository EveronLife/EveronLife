enum EL_EDbOperationStatusCode
{
	SUCCESS,

	// System failure
	FAILURE_DB_UNAVAILABLE,

	// User failure
	FAILURE_DATA_MALFORMED,
	FAILURE_ID_NOT_SET,
	FAILURE_ID_NOT_FOUND,

	// Unknown
	FAILURE_UNKNOWN
};

class EL_DbFindResultBase
{
	protected EL_EDbOperationStatusCode m_eStatusCode;

	//------------------------------------------------------------------------------------------------
	EL_EDbOperationStatusCode GetStatusCode()
	{
		return m_eStatusCode;
	}

	//------------------------------------------------------------------------------------------------
	bool IsSuccess()
	{
		return m_eStatusCode == EL_EDbOperationStatusCode.SUCCESS;
	}
};

class EL_DbFindResultMultiple<Class TEntityType> : EL_DbFindResultBase
{
	protected ref array<ref TEntityType> m_aEntities;

	//------------------------------------------------------------------------------------------------
	array<ref TEntityType> GetEntities()
	{
		return m_aEntities;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindResultMultiple(EL_EDbOperationStatusCode statusCode, array<ref TEntityType> entities = null)
	{
		m_eStatusCode = statusCode;
		m_aEntities = entities;
	}
};

class EL_DbFindResultSingle<Class TEntityType> : EL_DbFindResultBase
{
	protected ref TEntityType m_pEntity;

	//------------------------------------------------------------------------------------------------
	TEntityType GetEntity()
	{
		return m_pEntity;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindResultSingle(EL_EDbOperationStatusCode statusCode, TEntityType entity = null)
	{
		m_eStatusCode = statusCode;
		m_pEntity = entity;
	}
};

class EL_DbOperationCallback : EL_Callback
{
};

class EL_DbOperationStatusOnlyCallback : EL_DbOperationCallback
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EL_EDbOperationStatusCode resultCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed void Invoke(EL_EDbOperationStatusCode code)
	{
		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, true, null, code, m_pContext)) return;

		if (code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};

class EL_DbFindCallbackBase : EL_DbOperationCallback
{
	//------------------------------------------------------------------------------------------------
	void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults);
};

class EL_DbFindCallbackMultiple<Class TEntityType> : EL_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(array<ref TEntityType> resultData, Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EL_EDbOperationStatusCode resultCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		array<ref TEntityType> strongTypedResults = EL_RefArrayCaster<EL_DbEntity, TEntityType>.Convert(findResults);

		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, true, null, code, strongTypedResults, m_pContext)) return;

		if (code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(strongTypedResults, m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};

class EL_DbFindCallbackSingle<Class TEntityType> : EL_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(TEntityType resultData, Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EL_EDbOperationStatusCode resultCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		TEntityType typedResult;

		if (findResults.Count() > 0)
			typedResult = TEntityType.Cast(findResults.Get(0));

		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, true, null, code, typedResult, m_pContext)) return;

		if (code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(typedResult, m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};

class EL_DbFindCallbackSingleton<Class TEntityType> : EL_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(TEntityType resultData, Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EL_EDbOperationStatusCode resultCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
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
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, true, null, code, typedResult, m_pContext)) return;

		if (code == EL_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(typedResult, m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};
