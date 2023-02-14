typedef int EL_DateTimeUtcAsInt;
class EL_DateTimeUtcAsInt
{
	//------------------------------------------------------------------------------------------------
	int GetSecond()
	{
		return value & 0x3f;
	}

	//------------------------------------------------------------------------------------------------
	int GetMinute()
	{
		return (value >> 6) & 0x3f;
	}

	//------------------------------------------------------------------------------------------------
	int GetHour()
	{
		return (value >> 12) & 0x1f;
	}

	//------------------------------------------------------------------------------------------------
	int GetDay()
	{
		return (value >> 17) & 0x1f;
	}

	//------------------------------------------------------------------------------------------------
	int GetMonth()
	{
		return (value >> 22) & 0xf;
	}

	//------------------------------------------------------------------------------------------------
	int GetYear()
	{
		return ((value >> 26) & 0x3f) + 2000;
	}

	//------------------------------------------------------------------------------------------------
	//! Gets the current UTC date time
	//! \return integer representation of the datetime
	static EL_DateTimeUtcAsInt Now()
	{
		int year, month, day, hour, minute, second;
		System.GetYearMonthDayUTC(year, month, day);
		System.GetHourMinuteSecondUTC(hour, minute, second);
		return ((year - 2000) << 26) | (month << 22) | (day << 17) | (hour << 12) | (minute << 6) | second;
	}
}
