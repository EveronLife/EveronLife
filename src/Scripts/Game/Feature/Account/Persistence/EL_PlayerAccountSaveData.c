[
	EPF_PersistentScriptedStateSettings(EL_PlayerAccount),
	EDF_DbName.Automatic()
]
class EL_PlayerAccountSaveData : EPF_ScriptedStateSaveData
{
	ref array<ref EL_PlayerCharacter> m_aCharacters = {};
	int m_iActiveCharacterIdx;

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EPF_ScriptedStateSaveData other)
	{
		EL_PlayerAccountSaveData otherData = EL_PlayerAccountSaveData.Cast(other);

		if (m_iActiveCharacterIdx != otherData.m_iActiveCharacterIdx)
			return false;

		if (m_aCharacters.Count() != otherData.m_aCharacters.Count())
			return false;

		foreach (int idx, EL_PlayerCharacter character : m_aCharacters)
		{
			// Try same index first as they are likely to be the correct ones.
			if (IsCharacterEqual(character, otherData.m_aCharacters.Get(idx)))
				continue;

			bool found;
			foreach (int compareIdx, EL_PlayerCharacter otherCharacter : otherData.m_aCharacters)
			{
				if (compareIdx == idx)
					continue; // Already tried in idx direct compare

				if (IsCharacterEqual(character, otherCharacter))
				{
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsCharacterEqual(EL_PlayerCharacter a, EL_PlayerCharacter b)
	{
		return (a.GetId() == b.GetId()) && (a.GetPrefab() == b.GetPrefab());
	}
};
