[EL_DbName(EL_WhitelistDbEntity, "Whitelist")]
class EL_WhitelistDbEntity : EL_DbEntity
{
	string m_sUuid;

	int m_iYear;
	int m_iMonth;
	int m_iDay;
	int m_iHour;
	int m_iMinute;
	int m_iSecond;

	string Print()
	{
		string message = string.Format("Uuid : %1 | Last Login : %2",m_sUuid, PrintTime());
		return message;
	}

	string PrintTime()
	{
		string message = string.Format("%1-%2-%3 %4H%5.%6",m_iDay ,m_iMonth ,m_iYear ,m_iHour ,m_iMinute ,m_iSecond);
		return message;
	}
}