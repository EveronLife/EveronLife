[EntityEditorProps(category: "EveronLife/Core", description: "Global storage manager for ground item management. Requires the matching component to be attached")]
class EL_GlobalBankAccountManagerClass : GenericEntityClass
{
}

class EL_GlobalBankAccountManager : GenericEntity
{
	protected ref array<ref EL_BankAccount> m_aBankAccounts = new array<ref EL_BankAccount>;
	
	protected static EL_GlobalBankAccountManager s_pInstance;
		
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
		int playerAccountId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
		foreach (ref EL_BankAccount account : m_aBankAccounts)
		{
			if (account.GetId() == playerAccountId)
				return account;
		}
		
		return CreateBankAccount(player);
	}
	
	//------------------------------------------------------------------------------------------------
	EL_BankAccount GetPlayerBankAccount(int playerAccountId)
	{
		foreach (ref EL_BankAccount account : m_aBankAccounts)
		{
			if (account.GetId() == playerAccountId)
				return account;
		}
		
		return CreateBankAccount(playerAccountId);
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
		EL_BankAccount newAccount = new EL_BankAccount(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player), 1000);
		m_aBankAccounts.Insert(newAccount);
		
		return newAccount;
	}
		
	//------------------------------------------------------------------------------------------------
	EL_BankAccount CreateBankAccount(int playerId)
	{
		EL_BankAccount newAccount = new EL_BankAccount(playerId, 1000);
		m_aBankAccounts.Insert(newAccount);
		
		return newAccount;
	}

	//------------------------------------------------------------------------------------------------	
	void EL_GlobalBankAccountManager(IEntitySource src, IEntity parent) 
	{
		s_pInstance = this;
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, true);
	}
}