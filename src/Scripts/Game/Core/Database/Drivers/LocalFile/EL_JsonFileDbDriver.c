[EL_DbDriverName(EL_JsonFileDbDriver, {"JsonFile", "Json"})]
class EL_JsonFileDbDriver : EL_FileDbDriverBase
{
	override protected string GetFileExtension()
	{
		return ".json";
	}
	
	override protected EL_EDbOperationStatusCode WriteToDisk(EL_DbEntity entity)
	{
		SCR_JsonSaveContext writer();
		if (!writer.WriteValue("entity", entity))
		{
			return EL_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;
		}
		
		if (!writer.SaveToFile(string.Format("%1/%2/%3.json", m_sDbDir, entity.Type().ToString(), entity.GetId())))
		{
			return EL_EDbOperationStatusCode.FAILURE_STORAGE_UNAVAILABLE;
		}
		
		return EL_EDbOperationStatusCode.SUCCESS;
	}
	
	override protected EL_EDbOperationStatusCode ReadFromDisk(typename entityType, string entityId, out EL_DbEntity entity)
	{
		string file = string.Format("%1/%2/%3.json", m_sDbDir, entityType, entityId);
		if (!FileIO.FileExist(file)) return EL_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND;

		SCR_JsonLoadContext reader();
		if (!reader.LoadFromFile(file)) return EL_EDbOperationStatusCode.FAILURE_STORAGE_UNAVAILABLE;
		
		entity = EL_DbEntity.Cast(entityType.Spawn());
		if (!reader.ReadValue("entity", entity)) return EL_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;
		
		return EL_EDbOperationStatusCode.SUCCESS;
	}
}
