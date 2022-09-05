[EL_DbName(EL_DbEntity, "DbEntity")]
class EL_DbEntity
{
	static const string FIELD_ID = "m_sId";

	private string m_sId;

	//------------------------------------------------------------------------------------------------
	//! Get the unique id of the db entity. Guaranteed to remain a string in any future version
	string GetId()
	{
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Set the unique id on the db entity. Must always be a string!
	void SetId(string id)
	{
		m_sId = id;
	}

	//------------------------------------------------------------------------------------------------
	//! Check if an id has been assigned
	bool HasId()
	{
		return m_sId;
	}
}

class EL_DbName
{
	protected static ref map<typename, string> s_mMapping;

	//------------------------------------------------------------------------------------------------
	static void Set(typename entityType, string name)
	{
		if (!s_mMapping) s_mMapping = new map<typename, string>();
		s_mMapping.Set(entityType, name);
	}

	//------------------------------------------------------------------------------------------------
	static string Get(typename entityType)
	{
		if (!entityType) return string.Empty;

		if (!s_mMapping) s_mMapping = new map<typename, string>();

		string result = s_mMapping.Get(entityType);

		if (result.IsEmpty())
		{
			result = entityType.ToString();
			s_mMapping.Set(entityType, result);
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	static typename GetTypeByName(string name)
	{
		if (!s_mMapping) s_mMapping = new map<typename, string>();

		typename result = s_mMapping.GetKeyByValue(name);

		if (!result)
		{
			result = name.ToType();
			s_mMapping.Set(result, name);
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbName(typename entityType, string name)
	{
		Set(entityType, name);
	}

}
