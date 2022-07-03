// Use typedef and always EL_DbRecordId in code so if switched to a none primitve type the code does not break
typedef string EL_DbEntityId;

// Add placeholder class for highlighting and preperation for a none primitve id wrapper type.
sealed class EL_DbEntityId : string
{
	static string Generate()
	{
		return "UNIQUE_ID"; //todo move to driver?
	}
}

class EL_DbEntity
{
	EL_DbEntityId m_Id;
}
