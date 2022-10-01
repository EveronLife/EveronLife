class EL_MetaDataDbEntity : EL_DbEntity
{
	int m_iDataLayoutVersion = 1;
	EL_DateTimeUtcAsInt m_iLastSaved;

	//------------------------------------------------------------------------------------------------
	//! Utility function to write meta-data to serializer
	void WriteMetaData(notnull BaseSerializationSaveContext saveContext)
	{
		WriteId(saveContext);
		saveContext.WriteValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);
	}

	//------------------------------------------------------------------------------------------------
	//! Utility function to read meta-data from serializer
	void ReadMetaData(notnull BaseSerializationLoadContext loadContext)
	{
		ReadId(loadContext);
		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);
	}
}
