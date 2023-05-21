class EL_DbEntityUtils
{
	//------------------------------------------------------------------------------------------------
	static bool StructAutoCopy(notnull Managed from, notnull Class to)
	{
		SCR_JsonSaveContext writer();
		if (!writer.WriteValue("", from))
			return false;

		string data = writer.ExportToString();

		SCR_JsonLoadContext reader();
		if (!reader.ImportFromString(data))
			return false;

		return reader.ReadValue("", to);
	}
};
