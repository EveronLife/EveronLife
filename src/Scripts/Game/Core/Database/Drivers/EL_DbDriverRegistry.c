class EL_DbDriverName
{
	//------------------------------------------------------------------------------------------------
	void EL_DbDriverName(typename driverType, TStringArray driverAliases = null)
	{
		EL_DbDriverRegistry.Register(driverType.ToString(), driverType);

		if (driverAliases)
		{
			foreach (string alias : driverAliases)
			{
				EL_DbDriverRegistry.Register(alias, driverType);
			}
		}
	}
}

class EL_DbDriverRegistry
{
	protected static ref map<string, typename> m_Drivers;

	//------------------------------------------------------------------------------------------------
	static void Register(string driverName, typename driverType)
	{
		if (!m_Drivers)
		{
			m_Drivers = new map<string, typename>();
		}

		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		m_Drivers.Set(driverNameInvariant, driverType);
	}

	//------------------------------------------------------------------------------------------------
	static void Unregister(string driverName)
	{
		if (!m_Drivers) return;

		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		m_Drivers.Remove(driverNameInvariant);
	}

	//------------------------------------------------------------------------------------------------
	static typename Get(string driverName)
	{
		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		if (!m_Drivers || !m_Drivers.Contains(driverNameInvariant)) return typename.Empty;

		return m_Drivers.Get(driverNameInvariant);
	}

	//------------------------------------------------------------------------------------------------
	static set<typename> GetAll()
	{
		set<typename> result();

		if (m_Drivers)
		{
			for (int nElement = 0; nElement < m_Drivers.Count(); nElement++)
			{
				result.Insert(m_Drivers.GetElement(nElement));
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		delete m_Drivers;
	}
}
