class EL_DbName
{
	protected static ref map<typename, string> m_Mapping;
	
	void EL_DbName(typename entityType, string name)
	{
		Set(entityType, name);
	}
	
	static void Set(typename entityType, string name)
	{
		if(!m_Mapping) m_Mapping = new map<typename, string>();
		m_Mapping.Set(entityType, name);
	}
	
	static string Get(typename entityType)
	{
		//if(!entityType) return string.Empty;
		
		if(!m_Mapping) m_Mapping = new map<typename, string>();
		
		string result = m_Mapping.Get(entityType);
		
		if(result.IsEmpty())
		{
			result = entityType.ToString();
			m_Mapping.Set(entityType, result);
		}
		
		return result;
	}
	
	static typename GetTypeByName(string name)
	{
		if(!m_Mapping) m_Mapping = new map<typename, string>();
		
		typename result = m_Mapping.GetKeyByValue(name);
		
		if(!result)
		{
			result = name.ToType();
			m_Mapping.Set(result, name);
		}
		
		return result;
	}
}

[EL_DbName(EL_DbEntity, "DbEntity")]
class EL_DbEntity
{
	static const string FIELD_ID = "m_sId";
	
	private string m_sId;
	
	string GetId()
	{
		return m_sId;
	}
	
	void SetId(string id)
	{
		m_sId = id;
	}
	
	bool HasId()
	{
		return m_sId;
	}
}
