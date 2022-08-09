[EL_ScriptedStateSaveDataType(EL_PlayerAccountSaveData, EL_PlayerAccount, true, false, "Account")]
class EL_PlayerAccountSaveData : EL_ScriptedStateSaveDataBase
{
	ref array<string> m_aCharacterIds;
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("dataLayoutVersion", 1);
		
		saveContext.WriteValue("m_sId", GetId());
		
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);

		saveContext.WriteValue("m_aCharacterIds", m_aCharacterIds);

		return true;
	}

	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;
		
		int dataLayoutVersion;
		loadContext.ReadValue("dataLayoutVersion", dataLayoutVersion);
		
		string id;
		loadContext.ReadValue("m_sId", id);
		SetId(id);
		
		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);
		
		loadContext.ReadValue("m_aCharacterIds", m_aCharacterIds);
		
		return true;
	}
}
