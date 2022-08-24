class EL_PlayerAccountManager
{
	protected static ref EL_PlayerAccountManager s_Instance;
	
	protected ref map<string, ref EL_PlayerAccount> m_mAccounts;
	
	EL_PlayerAccount GetAccount(string playerUid, bool create = false)
	{
		EL_PlayerAccount account = m_mAccounts.Get(playerUid);
		if(!account)
		{
			// Try and get the account from persistence data
			account = EL_PersistentScriptedStateLoader<EL_PlayerAccount>.Get(playerUid);
			if(!account && create)
			{
				// Setup a new account for this uid
				account = EL_PlayerAccount.Create(playerUid);
				PrintFormat("Created new account for UID '%1'.", playerUid);
			}
			
			// Cache account
			if(account) m_mAccounts.Set(account.GetPersistentId(), account);
		}
		
		return account;
	}
	
	EL_PlayerAccount GetAccount(IEntity player)
	{
		return GetAccount(EL_Utils.GetPlayerUID(player));
	}
	
	void UnloadAccount(string playerUid)
	{
		EL_PlayerAccount account = m_mAccounts.Get(playerUid);
		if(!account) return;
		
		account.Save();
		account.Detach();
		m_mAccounts.Remove(playerUid);
	}
	
	static EL_PlayerAccountManager GetInstance()
	{
		if(!s_Instance) s_Instance = new EL_PlayerAccountManager();
		
		return s_Instance;
	}
	
	static void Reset()
	{
		s_Instance = null;
	}
	
	protected void EL_PlayerAccountManager()
	{
		m_mAccounts = new map<string, ref EL_PlayerAccount>()
	}
}
