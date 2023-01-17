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
	EL_BankAccount GetLocalPlayerBankAccount()
	{
		return m_LocalBankAccount;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	void LoadPlayerBankAccount(IEntity player, int playerAccountId)
	{
		Print("[EL-Bank] 3. Loading account for " + playerAccountId);
		
		EL_BankAccount bankAccount = GetBankAccount(playerAccountId);
		if (!bankAccount)
		{
			Print("[EL-Bank] 3.1 Trying to loading non existant account for " + playerAccountId + " -> creating new one");
			bankAccount = CreateBankAccount(playerAccountId);
		}
		
		RplComponent rplC = EL_ComponentFinder<RplComponent>.Find(player);
		if (rplC.IsMaster())
		{
			SetPlayerBankAccount(rplC.Id(), bankAccount);
		}	
		else
		{
			Rpc(SetPlayerBankAccount, rplC.Id(), bankAccount);
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void SetPlayerBankAccount(RplId targetPlayerId, EL_BankAccount account)
	{
		Print("[EL-Bank] 3.2 Got Broadcast " + targetPlayerId);
		RplComponent targetRplComponent = RplComponent.Cast(Replication.FindItem(targetPlayerId));
		IEntity targetPlayer = targetRplComponent.GetEntity();
		IEntity localPlayer = SCR_PlayerController.GetLocalControlledEntity();
		
		//Broadcast not for this player
		if (targetPlayer != localPlayer)
			return;
		Print("[EL-Bank] 3.3 Is for me");
		m_LocalBankAccount = account;
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
		EL_BankAccount newAccount = EL_BankAccount.Create(playerId, 1000);
		m_aBankAccounts.Insert(newAccount);
		Print("[EL-Bank] 3.1.1. Created account " + newAccount.GetId());
		return newAccount;
	}
	
	//------------------------------------------------------------------------------------------------
	EL_BankAccount GetBankAccount(int playerId)
	{
		foreach (EL_BankAccount account : m_aBankAccounts)
		{
			if (account.GetId() == playerId)
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