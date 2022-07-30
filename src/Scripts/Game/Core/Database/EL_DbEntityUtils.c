class EL_DbEntityUtils
{
	static void ApplyDbEntityTo(EL_DbEntity entity, notnull Class other)
	{
		SCR_JsonSaveContext writer();
		writer.WriteValue("entity", entity);
		
		SCR_JsonLoadContext reader();
		reader.ImportFromString(writer.ExportToString());
		reader.ReadValue("entity", other);
	}
}
