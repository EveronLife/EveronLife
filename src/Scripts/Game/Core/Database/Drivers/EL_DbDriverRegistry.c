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
	protected static ref map<string, typename> s_mDrivers;

	//------------------------------------------------------------------------------------------------
	static void Register(string driverName, typename driverType)
	{
		if (!s_mDrivers)
		{
			s_mDrivers = new map<string, typename>();
		}

		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		s_mDrivers.Set(driverNameInvariant, driverType);
	}

	//------------------------------------------------------------------------------------------------
	static void Unregister(string driverName)
	{
		if (!s_mDrivers) return;

		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		s_mDrivers.Remove(driverNameInvariant);
	}

	//------------------------------------------------------------------------------------------------
	static typename Get(string driverName)
	{
		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		if (!s_mDrivers || !s_mDrivers.Contains(driverNameInvariant)) return typename.Empty;

		return s_mDrivers.Get(driverNameInvariant);
	}

	//------------------------------------------------------------------------------------------------
	static set<typename> GetAll()
	{
		set<typename> result();

		if (s_mDrivers)
		{
			for (int nElement = 0; nElement < s_mDrivers.Count(); nElement++)
			{
				result.Insert(s_mDrivers.GetElement(nElement));
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		delete s_mDrivers;
	}
}
