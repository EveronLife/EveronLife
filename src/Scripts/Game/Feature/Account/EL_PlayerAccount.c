[EL_PersistentScriptedStateSettings(EL_PlayerAccount, EL_PlayerAccountSaveData, saveType: EL_ESaveType.INTERVAL_SHUTDOWN, selfDelete: false)]
class EL_PlayerAccount : EL_PersistentScriptedState
{
	ref array<string> m_aCharacterIds;

	//------------------------------------------------------------------------------------------------
	static EL_PlayerAccount Create(string playerUid)
	{
		EL_PlayerAccount account();
		account.SetPersistentId(playerUid);
		account.m_aCharacterIds = new array<string>();
		return account;
	}
}
