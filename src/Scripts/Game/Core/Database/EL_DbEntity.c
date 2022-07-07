class EL_DbEntity
{
	private string m_Id;
	
	string GetId()
	{
		if(!m_Id)
		{
			m_Id = EL_EntityIdGenerator.Generate();
		}
		
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
