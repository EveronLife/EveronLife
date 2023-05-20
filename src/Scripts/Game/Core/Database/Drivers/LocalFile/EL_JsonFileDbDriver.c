[EL_DbDriverName({"JsonFile", "Json"})]
class EL_JsonFileDbDriver : EL_FileDbDriverBase
{
	protected bool m_bPrettyPrint;

	//------------------------------------------------------------------------------------------------
	override bool Initalize(string connectionString = string.Empty)
	{
		bool result = super.Initalize(connectionString);
		m_bPrettyPrint = connectionString.Contains("pretty=true");
		return result;
	}

	//------------------------------------------------------------------------------------------------
	override protected string GetFileExtension()
	{
		return ".json";
	}

	//------------------------------------------------------------------------------------------------
	override protected EL_EDbOperationStatusCode WriteToDisk(EL_DbEntity entity)
	{
		ContainerSerializationSaveContext writer();
		BaseJsonSerializationSaveContainer jsonContainer;
		if (m_bPrettyPrint)
		{
			jsonContainer = new PrettyJsonSaveContainer();
		}
		else
		{
			jsonContainer = new JsonSaveContainer();
		}

		jsonContainer.SetMaxDecimalPlaces(5);
		writer.SetContainer(jsonContainer);

		if (!writer.WriteValue("", entity))
			return EL_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;

		if (!jsonContainer.SaveToFile(string.Format("%1/%2.json", _GetTypeDirectory(entity.Type()), entity.GetId())))
			return EL_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;

		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override protected EL_EDbOperationStatusCode ReadFromDisk(typename entityType, string entityId, out EL_DbEntity entity)
	{
		string file = string.Format("%1/%2.json", _GetTypeDirectory(entityType), entityId);
		if (FileIO.FileExist(file))
		{
			SCR_JsonLoadContext reader();
			if (!reader.LoadFromFile(file))
				return EL_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;

			entity = EL_DbEntity.Cast(entityType.Spawn());
			if (!reader.ReadValue("", entity))
				return EL_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;
		}

		return EL_EDbOperationStatusCode.SUCCESS;
	}
};
