class EL_PlayerAccountManager
{
	protected static ref EL_PlayerAccountManager s_pInstance;
	protected ref map<string, ref EL_PlayerAccount> m_mAccounts;

	//------------------------------------------------------------------------------------------------
	//! Async loading of a players account
	//! \param playerUid The players Bohemia UID
	//! \param create If true a new account will be created if none is found for the UID
	//! \param callback Async callback to handle the result
	void LoadAccountAsync(string playerUid, bool create, notnull EL_PlayerAccountCallback callback)
	{
		EL_PlayerAccount account = m_mAccounts.Get(playerUid);
		if (account)
		{
			callback.Invoke(account);
			return;
		}

		auto processorCallback = EL_PlayerAccountManagerProcessorCallback.Create(playerUid, create, callback);
		EL_PersistentScriptedStateLoader<EL_PlayerAccount>.LoadAsync(playerUid, processorCallback);
	}

	//------------------------------------------------------------------------------------------------
	//! Save and pending changes on the account and release it from the manager. Used primarily on disconnect of player
	//! \param playerUid The players Bohemia UID
	void SaveAndReleaseAccount(string playerUid)
	{
		EL_PlayerAccount account = m_mAccounts.Get(playerUid);
		if (!account) return;

		account.Save();
		account.Detach();
		m_mAccounts.Remove(playerUid);
	}

	//------------------------------------------------------------------------------------------------
	//! Add the player accountt instance to the cache so it is returned on the next LoadAccountAsync call
	//! \param account Account instance to cache
	void AddToCache(notnull EL_PlayerAccount account)
	{
		m_mAccounts.Set(account.GetPersistentId(), account);
	}

	//------------------------------------------------------------------------------------------------
	EL_PlayerAccount GetFromCache(string playerUid)
	{
		return m_mAccounts.Get(playerUid);
	}

	//------------------------------------------------------------------------------------------------
	EL_PlayerAccount GetFromCache(IEntity player)
	{
		return GetFromCache(EL_Utils.GetPlayerUID(player));
	}

	//------------------------------------------------------------------------------------------------
	static EL_PlayerAccountManager GetInstance()
	{
		if (!s_pInstance) s_pInstance = new EL_PlayerAccountManager();
		return s_pInstance;
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_pInstance = null;
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_PlayerAccountManager()
	{
		m_mAccounts = new map<string, ref EL_PlayerAccount>()
	}
}

class EL_PlayerAccountManagerProcessorCallback : EL_ScriptedStateLoaderCallbackSingle<EL_PlayerAccount>
{
	string m_sPlayerUid
	bool m_bCreate;
	ref EL_PlayerAccountCallback m_pCallback;

	//------------------------------------------------------------------------------------------------
	override void OnComplete(Managed context, EL_PlayerAccount data)
	{
		EL_PlayerAccount result = data; //Keep explicit strong ref to it or else create on null will fail
		if (!result && m_bCreate) result = EL_PlayerAccount.Create(m_sPlayerUid);
		if (result) EL_PlayerAccountManager.GetInstance().AddToCache(result);
		if (m_pCallback) m_pCallback.Invoke(result);
	}

	//------------------------------------------------------------------------------------------------
	static EL_PlayerAccountManagerProcessorCallback Create(string playerUid, bool create, EL_PlayerAccountCallback callback)
	{
		EL_PlayerAccountManagerProcessorCallback instance();
		instance.m_sPlayerUid = playerUid;
		instance.m_bCreate = create;
		instance.m_pCallback = callback;
		return instance;
	}
}

class EL_PlayerAccountCallback : EL_ScriptedStateLoaderCallbackSingle<EL_PlayerAccount>
{
}
