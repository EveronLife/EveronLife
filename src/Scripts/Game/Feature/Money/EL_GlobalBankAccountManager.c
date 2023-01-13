[EntityEditorProps(category: "EveronLife/Core", description: "Global storage manager for ground item management. Requires the matching component to be attached")]
class EL_GlobalBankAccountManagerClass : GenericEntityClass
{
}

class EL_GlobalBankAccountManager : GenericEntity
{
	protected ref array<ref EL_BankAccount> m_aBankAccounts;
	
	protected static EL_GlobalBankAccountManager s_pInstance;
	EL_BankAccount m_LocalBankAccount;
	
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
	EL_BankAccount GetLocalPlayerBankAccount()
	{
		return GetPlayerBankAccount(SCR_PlayerController.GetLocalControlledEntity());
	}
	
	//------------------------------------------------------------------------------------------------
	EL_BankAccount GetPlayerBankAccount(IEntity player)
	{
		//return GetPlayerBankAccount(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	void LoadPlayerBankAccount(int playerAccountId)
	{
		foreach (EL_BankAccount account : m_aBankAccounts)
		{
			if (account.GetId() == playerAccountId)
				Rpc(SetPlayerBankAccount, account);
		}
		
		//return CreateBankAccount(playerAccountId);
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void SetPlayerBankAccount(EL_BankAccount account)
	{
		m_LocalBankAccount = account;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetBankAccounts(array<ref EL_BankAccount> bankAccounts)
	{
		Print("Loading Bank accounts persisent..");
		m_aBankAccounts = bankAccounts;
		
		foreach (EL_BankAccount account : m_aBankAccounts)
		{
			Print("transactions in account: " + account.m_aTransactions.Count());
		}
	}	
		
	//------------------------------------------------------------------------------------------------
	array<ref EL_BankAccount> GetBankAccounts()
	{
		return m_aBankAccounts;
	}	
	
	//------------------------------------------------------------------------------------------------
	EL_BankAccount CreateBankAccount(IEntity player)
	{
		EL_BankAccount newAccount = EL_BankAccount.Create(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player), 1000);
		m_aBankAccounts.Insert(newAccount);
		
		return newAccount;
	}
		
	//------------------------------------------------------------------------------------------------
	ref EL_BankAccount CreateBankAccount(int playerId)
	{
		EL_BankAccount newAccount = EL_BankAccount.Create(playerId, 1000);
		m_aBankAccounts.Insert(newAccount);
		
		return newAccount;
	}

	//------------------------------------------------------------------------------------------------	
	void EL_GlobalBankAccountManager(IEntitySource src, IEntity parent) 
	{
		s_pInstance = this;
		m_aBankAccounts = new array<ref EL_BankAccount>();
		
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, false);
	}
}