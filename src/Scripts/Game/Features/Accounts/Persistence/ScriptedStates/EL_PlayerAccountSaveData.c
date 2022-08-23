[EL_DbName(EL_PlayerAccountSaveData, "Account")]
class EL_PlayerAccountSaveData : EL_ScriptedStateSaveDataBase
{
	ref array<string> m_aCharacterIds;
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		
		saveContext.WriteValue("m_sId", GetId());
		
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);

		saveContext.WriteValue("m_aCharacterIds", m_aCharacterIds);

		return true;
	}

	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;
		
		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		
		string id;
		loadContext.ReadValue("m_sId", id);
		SetId(id);
		
		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);
		
		loadContext.ReadValue("m_aCharacterIds", m_aCharacterIds);
		
		return true;
	}
}
