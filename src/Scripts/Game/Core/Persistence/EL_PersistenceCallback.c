class EL_PersistenceLoaderCallback : EL_Callback
{
}

class EL_PersistenceLoaderCallbackSingle<Class T> : EL_PersistenceLoaderCallback
{
	void Invoke(T data)
	{
		if (m_pInvokeInstance && 
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, data)) return;
		
		OnComplete(m_pContext, data);
	}
	
	void OnComplete(Managed context, T data);
}

class EL_PersistenceLoaderCallbackMultiple<Class T> : EL_PersistenceLoaderCallback
{
	void Invoke(array<ref T> data)
	{
		if (m_pInvokeInstance && 
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext, data)) return;
		
		OnComplete(m_pContext, data);
	}
	
	void OnComplete(Managed context, array<ref T> data);
}
