[EL_DbDriverName({"BinaryFile", "BinFile", "Bin"})]
class EL_BinaryFileDbDriver : EL_FileDbDriverBase
{
	//------------------------------------------------------------------------------------------------
	override protected string GetFileExtension()
	{
		return ".bin";
	}

	//------------------------------------------------------------------------------------------------
	override protected EL_EDbOperationStatusCode WriteToDisk(EL_DbEntity entity)
	{
		SCR_BinSaveContext writer();
		if (!writer.WriteValue("", entity))
			return EL_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;

		if (!writer.SaveToFile(string.Format("%1/%2.bin", _GetTypeDirectory(entity.Type()), entity.GetId())))
			return EL_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;

		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override protected EL_EDbOperationStatusCode ReadFromDisk(typename entityType, string entityId, out EL_DbEntity entity)
	{
		string file = string.Format("%1/%2.bin", _GetTypeDirectory(entityType), entityId);
		if (FileIO.FileExist(file))
		{
			SCR_BinLoadContext reader();
			if (!reader.LoadFromFile(file))
				return EL_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;
			
			entity = EL_DbEntity.Cast(entityType.Spawn());
			if (!reader.ReadValue("", entity))
				return EL_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;
		}

		return EL_EDbOperationStatusCode.SUCCESS;
	}
};
