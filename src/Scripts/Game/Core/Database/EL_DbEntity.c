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
