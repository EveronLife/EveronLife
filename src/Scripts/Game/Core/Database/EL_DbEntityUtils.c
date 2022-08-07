class EL_DbEntityUtils
{
	static bool StructAutoCopy(notnull Class from, notnull Class to)
	{
		SCR_JsonSaveContext writer();
		writer.WriteValue("data", from);
		
		SCR_JsonLoadContext reader();
		reader.ImportFromString(writer.ExportToString());
		return reader.ReadValue("data", to);
	}
}
