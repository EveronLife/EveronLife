[EL_PersistentScriptedStateSettings(EL_PlayerAccount, EL_PlayerAccountSaveData, autoSave: false, shutDownSave: true, selfDelete: false)]
class EL_PlayerAccount : EL_PersistentScriptedStateBase
{
	ref array<string> m_aCharacterIds;
	
	void SetPlayerUid(string playerUid)
	{
		m_sId = playerUid;
	}
	
	static EL_PlayerAccount Create(string playerUid)
	{
		EL_PlayerAccount account();
		account.SetPlayerUid(playerUid);
		account.m_aCharacterIds = new array<string>();
		return account;
	}
}
