[
	EL_PersistentScriptedStateSettings(EL_PlayerAccount, saveType: EL_ESaveType.INTERVAL_SHUTDOWN, options: EL_EPersistentScriptedStateOptions.USE_CHANGE_TRACKER), 
	EL_DbName.Automatic()
]
class EL_PlayerAccountSaveData : EL_ScriptedStateSaveData
{
	ref array<string> m_aCharacterIds;

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ScriptedStateSaveData other)
	{
		EL_PlayerAccountSaveData otherData = EL_PlayerAccountSaveData.Cast(other);

		if (m_aCharacterIds.Count() != otherData.m_aCharacterIds.Count())
			return false;

		foreach (int idx, string characterId : m_aCharacterIds)
		{
			// Try same index first as they are likely to be the correct ones.
			if (characterId == otherData.m_aCharacterIds.Get(idx))
				continue;

			if (!otherData.m_aCharacterIds.Contains(characterId))
				return false;
		}

		return true;
	}
};
