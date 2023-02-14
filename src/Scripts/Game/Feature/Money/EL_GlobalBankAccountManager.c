[EntityEditorProps(category: "EveronLife/Core", description: "Global storage manager for ground item management. Requires the matching component to be attached")]
class EL_GlobalBankAccountManagerClass : GenericEntityClass
{
}

class EL_GlobalBankAccountManager : GenericEntity
{
	protected ref array<ref EL_BankAccount> m_aBankAccounts;

	protected static EL_GlobalBankAccountManager s_pInstance;
	ref EL_BankAccount m_LocalBankAccount;

	//------------------------------------------------------------------------------------------------
	static EL_GlobalBankAccountManager GetInstance()
	{
		return s_pInstance;
	}

	//------------------------------------------------------------------------------------------------
	void OpenBankMenu()
	{
		EL_BankMenu.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.EL_BankMenu));
	}

	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	//! TODO: Add transfer methode
	EL_BankAccount NewAccountTransaction(string playerUid, int amount, string comment)
	{
		EL_BankAccount sourceAccount = GetBankAccount(playerUid);
		if (amount == 0)
			return sourceAccount;
		if (amount > 0)
			sourceAccount.TryDeposit(amount, comment);
		else
			sourceAccount.TryWithdraw(-amount, comment);

		return sourceAccount;
	}

	//------------------------------------------------------------------------------------------------
	EL_BankAccount GetLocalPlayerBankAccount()
	{
		return m_LocalBankAccount;
	}

	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	void LoadPlayerBankAccount(IEntity player)
	{
		Print("[EL-Bank] Loading account for " + EL_Utils.GetPlayerUID(player));

		//Load or create account
		EL_BankAccount bankAccount = GetBankAccount(EL_Utils.GetPlayerUID(player));
		if (!bankAccount)
		{
			Print("[EL-Bank] Trying to loading non existant account for " + EL_Utils.GetPlayerUID(player) + " -> creating new one");
			bankAccount = CreateBankAccount(player);
		}

		//Send account data to client
		EL_RpcSenderComponent rpcSender = EL_ComponentFinder<EL_RpcSenderComponent>.Find(player);
		rpcSender.AskSetLocalBankAccount(bankAccount);
	}

	//------------------------------------------------------------------------------------------------
	void SetBankAccounts(array<ref EL_BankAccount> bankAccounts)
	{
		m_aBankAccounts = bankAccounts;
	}

	//------------------------------------------------------------------------------------------------
	array<ref EL_BankAccount> GetBankAccounts()
	{
		return m_aBankAccounts;
	}

	//------------------------------------------------------------------------------------------------
	EL_BankAccount CreateBankAccount(IEntity player)
	{
		return CreateBankAccount(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
	}

	//------------------------------------------------------------------------------------------------
	EL_BankAccount CreateBankAccount(int playerId)
	{
		EL_BankAccount newAccount = EL_BankAccount.Create(EL_Utils.GetPlayerUID(playerId), GetRandomFreeAccountId(), 1000);
		m_aBankAccounts.Insert(newAccount);
		Print("[EL-Bank] Created new account id: " + newAccount.GetAccountOwnerUid());
		return newAccount;
	}

	//------------------------------------------------------------------------------------------------
	//! Gets a random free 8 digit id
	int GetRandomFreeAccountId()
	{
		int newAccountId = Math.RandomInt(11111111, 99999999);
		foreach (EL_BankAccount account : m_aBankAccounts)
		{
			if (account.GetId() == newAccountId)
				return GetRandomFreeAccountId();
		}
		return newAccountId;
	}

	//------------------------------------------------------------------------------------------------
	EL_BankAccount GetBankAccount(string playerUid)
	{
		foreach (EL_BankAccount account : m_aBankAccounts)
		{
			if (account.GetAccountOwnerUid() == playerUid)
				return account;
		}
		return null;
	}

	//------------------------------------------------------------------------------------------------
	void EL_GlobalBankAccountManager(IEntitySource src, IEntity parent)
	{
		s_pInstance = this;
		m_aBankAccounts = new array<ref EL_BankAccount>();

		SetFlags(EntityFlags.ACTIVE, false);
	}
}