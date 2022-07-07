// Use typedef and always EL_DbRecordId in code so if switched to a none primitve type the code does not break
typedef string EL_DbEntityId;

// Add placeholder class for highlighting and preperation for a none primitve id wrapper type.
sealed class EL_DbEntityId : string
{
	private static int m_Sequence;
	
	static string Generate()
	{
		return string.Format("UNIQUE_%1", m_Sequence++); //todo move to driver?
	}
}

class EL_DbEntity
{
	private EL_DbEntityId m_Id;
	
	EL_DbEntityId GetId()
	{
		if(!m_Id)
		{
			m_Id = EL_DbEntityId.Generate();
		}
		
		return m_Id;
	}
	
	void SetId(EL_DbEntityId id)
	{
		m_Id = id;
	}
	
	bool HasId()
	{
		return m_Id;
	}
}
