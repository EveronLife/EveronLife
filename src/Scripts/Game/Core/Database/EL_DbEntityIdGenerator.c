class EL_EntityIdGenerator
{
	protected static int m_Sequence;
	
	protected static ref RandomGenerator m_Random = new RandomGenerator();
	
	static string Generate()
	{
		int year, month, day, hour, minute, second;
		System.GetYearMonthDayUTC(year, month, day);
		System.GetHourMinuteSecondUTC(hour, minute, second);
		DateTimeUtcAsInt timestamp = ((year - 2000) << 26) | (month << 22) | (day << 17) | (hour << 12) | (minute << 6) | second;
		
		//No need to look at past generated ids in db for conflict, because they have older timestamps
		string timeHex = EL_Utils.IntToHex(timestamp);
		
		//Always 1 until hive system is implemented. Makes conflicts across hives impossible
		string hiveHex = "00000001"; 
		
		//Sequence returns to 0 on overflow
		string seqHex = EL_Utils.IntToHex(m_Sequence++);
		if(m_Sequence == int.MAX) m_Sequence = 0;
		
		//Random component to ensure that even if all sequentials are generated in the same second it stays unique
		string randHex = EL_Utils.IntToHex((int)(m_Random.RandFloatXY(0, int.MAX)));
		
		return string.Format("%1-%2-%3-%4-%5%6", timeHex, hiveHex.Substring(0, 4), hiveHex.Substring(4, 4), seqHex.Substring(0, 4), seqHex.Substring(4, 4), randHex);
	}
}
