//! Base class for scripted callbacks. Primarily used for async operations
class EL_Callback
{
	ref Managed m_pContext;
	Class m_pInvokeInstance;
	string m_sInvokeMethod;

	//------------------------------------------------------------------------------------------------
	void Invoke()
	{
		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, true, null, m_pContext)) return;
	}

	//------------------------------------------------------------------------------------------------
	void EL_Callback(Class invokeInstance = null, string invokeMethod = string.Empty, Managed context = null)
	{
		m_pInvokeInstance = invokeInstance;
		m_sInvokeMethod = invokeMethod;
		m_pContext = context;
	}
};
