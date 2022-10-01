class EL_DbEntityUtils
{
	//------------------------------------------------------------------------------------------------
	static bool StructAutoCopy(notnull Managed from, notnull Class to)
	{
		SCR_JsonSaveContext writer();
		writer.WriteValue("", from);

		SCR_JsonLoadContext reader();
		reader.ImportFromString(writer.ExportToString());
		return reader.ReadValue("", to);
	}
}
