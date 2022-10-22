class EL_DbEntityUtils
{
	//------------------------------------------------------------------------------------------------
	static bool StructAutoCopy(notnull Managed from, notnull Class to)
	{
		SCR_JsonSaveContext writer(false);
		writer.WriteValue("data", from);

		SCR_JsonLoadContext reader(false);
		reader.ImportFromString(writer.ExportToString());
		return reader.ReadValue("data", to);
	}
}
