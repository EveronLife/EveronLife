class EL_DbEntityName
{
	protected static ref map<typename, string> m_Mapping;
	
	void EL_DbEntityName(typename entityType, string name)
	{
		if(!m_Mapping) m_Mapping = new map<typename, string>();
		m_Mapping.Set(entityType, name);
	}
	
	static string Get(typename entityType)
	{
		if(!m_Mapping) m_Mapping = new map<typename, string>();
		
		string result = m_Mapping.Get(entityType);
		
		if(result.IsEmpty())
		{
			result = entityType.ToString();
			m_Mapping.Set(entityType, result);
		}
		
		return result;
	}
}

[EL_DbEntityName(EL_DbEntity, "DbEntity")]
class EL_DbEntity
{
	static const string FIELD_ID = "m_Id";
	
	private string m_Id;
	
	string GetId()
	{
		return m_Id;
	}
	
	void SetId(string id)
	{
		m_Id = id;
	}
	
	bool HasId()
	{
		return m_Id;
	}
}
