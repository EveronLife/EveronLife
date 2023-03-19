class EL_MetaDataDbEntity : EL_DbEntity
{
	int m_iDataLayoutVersion = 1;
	EL_DateTimeUtcAsInt m_iLastSaved;

	//------------------------------------------------------------------------------------------------
	//! Utility function to read meta-data
	void ReadMetaData(notnull EL_PersistenceComponent persistenceComponent)
	{
		SetId(persistenceComponent.GetPersistentId());
		m_iLastSaved = persistenceComponent.GetLastSaved();
	}

	//------------------------------------------------------------------------------------------------
	//! Utility function to read meta-data
	void ApplyMetaData(notnull EL_PersistenceComponent persistenceComponent)
	{
		// Id alraedy loaded on component through persistence manager
		persistenceComponent.SetLastSaved(m_iLastSaved);
	}

	//------------------------------------------------------------------------------------------------
	//! Utility function to write meta-data to serializer
	void SerializeMetaData(notnull BaseSerializationSaveContext saveContext)
	{
		WriteId(saveContext);
		saveContext.WriteValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);
	}

	//------------------------------------------------------------------------------------------------
	//! Utility function to read meta-data from serializer
	void DeserializeMetaData(notnull BaseSerializationLoadContext loadContext)
	{
		ReadId(loadContext);
		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);
	}
}
