[
	EPF_PersistentScriptedStateSettings(EL_PlayerAccount),
	EDF_DbName.Automatic()
]
class EL_PlayerAccountSaveData : EPF_ScriptedStateSaveData
{
	ref array<ref EL_PlayerCharacter> m_aCharacters = {};
	int m_iActiveCharacterIdx;

	//------------------------------------------------------------------------------------------------
	override EPF_EReadResult ReadFrom(notnull Managed scriptedState)
	{
		// TODO: Undo this and make the props protected again after https://feedback.bistudio.com/T174113 is fixed!
		EL_PlayerAccount account = EL_PlayerAccount.Cast(scriptedState);
		SetId(account.GetPersistentId());
		m_aCharacters = account.m_aCharacters;
		m_iActiveCharacterIdx = account.m_iActiveCharacterIdx;
		return EPF_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EPF_EApplyResult ApplyTo(notnull Managed scriptedState)
	{
		EL_PlayerAccount account = EL_PlayerAccount.Cast(scriptedState);
		account.SetPersistentId(GetId());
		account.m_aCharacters = m_aCharacters;
		account.m_iActiveCharacterIdx = m_iActiveCharacterIdx;
		return EPF_EReadResult.OK;
	}

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
}
