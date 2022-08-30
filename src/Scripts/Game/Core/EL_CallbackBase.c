//! Base class for scripted callbacks. Primarily used for async operations
class EL_Callback
{
	ref Managed m_pContext;
	Class m_pInvokeInstance;
	string m_sInvokeMethodName;

	//------------------------------------------------------------------------------------------------
	void ConfigureInvoker(notnull Class instance, string functionName)
	{
		m_pInvokeInstance = instance;
		m_sInvokeMethodName = functionName;
	}

	//------------------------------------------------------------------------------------------------
	void Invoke()
	{
		if (m_pInvokeInstance &&
			m_sInvokeMethodName &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethodName, true, null, m_pContext)) return;
	}

	//------------------------------------------------------------------------------------------------
	void EL_Callback(Managed context = null)
	{
		m_pContext = context;
	}
}
