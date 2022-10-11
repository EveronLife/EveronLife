[EL_DbName(EL_WhitelistDbEntity, "Whitelist")]
class EL_WhitelistDbEntity : EL_DbEntity
{
	string m_sUuid;
	EL_WhitelistTimeDbEntity m_pLastLogin;
	EPlatform m_pPlatform;
}

class EL_WhitelistTimeDbEntity
{
	int m_iYear;
	int m_iMonth;
	int m_iDay;
	int m_iHour;
	int m_iMinute;
	int m_iSecond;
}