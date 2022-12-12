class EL_FormatUtils
{
	//------------------------------------------------------------------------------------------------
	static string AbbreviateNumber(int number)
	{
		string result;
		if (number >= 1000000000)
		{
			result = string.Format("%1B", (number / 1000000000.0).ToString(-1, 1));
		}
		else if (number >= 1000000)
		{
			result = string.Format("%1M", (number / 1000000.0).ToString(-1, 1));
		}
		else if (number >= 1000)
		{
			result = string.Format("%1K", (number / 1000.0).ToString(-1, 1));
		}
		else
		{
			result = number.ToString();
		}

		return result;
	}
}
