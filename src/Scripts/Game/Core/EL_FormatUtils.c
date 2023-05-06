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

	//------------------------------------------------------------------------------------------------
	//! Int to decimal seperated format (eg.: 9.999.999)
	//! \param number Number to convert
	static string DecimalSeperator(int number)
	{
		string numberText = number.ToString();
		string formatNumberText;

		int dotAmount = (numberText.Length() - 1) / 3;
		for (int i=0; i < dotAmount; i++)
		{
			formatNumberText = string.Format(".%1%2", numberText.Substring(numberText.Length() - ((i + 1) * 3), 3), formatNumberText);
		}
		formatNumberText = string.Format("%1%2", numberText.Substring(0, numberText.Length() - dotAmount * 3), formatNumberText);

		return formatNumberText;
	}
}
