[EntityEditorProps(category: "EveronLife/Core")]
class EL_RpcSenderComponentClass : ScriptComponentClass
{
}

class EL_RpcSenderComponent  : ScriptComponent
{
	protected EL_GlobalBankAccountManager m_BankManager;
	protected bool m_bIsMaster;
	
	//------------------------------------------------------------------------------------------------
	void AskSetLocalBankAccount(EL_BankAccount account)
	{
		Rpc(Rpc_DoSetLocalBankAccount, account);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void Rpc_DoSetLocalBankAccount(EL_BankAccount account)
	{	
		m_BankManager.m_LocalBankAccount = account;
		//Update open bank ui
		EL_BankMenu bankMenu = EL_BankMenu.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.EL_BankMenu));
		if (bankMenu)
			bankMenu.OnMenuFocusGained();
	}
	
	//------------------------------------------------------------------------------------------------
	void AskTransactionFromBankAccount(int amount)
	{
		if (m_bIsMaster)
			Rpc_DoTransactionBankAccount(amount);
		else
			Rpc(Rpc_DoTransactionBankAccount, amount);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_DoTransactionBankAccount(int amount)
	{
		//TODO: Change this to unique uuid
		int accountId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetOwner());
		EL_BankAccount updatedAccount = m_BankManager.NewAccountTransaction(accountId, amount);
		
		//Send updated account back to client
		AskSetLocalBankAccount(updatedAccount);
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_DoLoadBankAccount()
	{
		//TODO: Change this to unique uuid
		m_BankManager.LoadPlayerBankAccount(GetOwner(), GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Init after Replication
	void LateInit(IEntity owner)
	{
		m_bIsMaster = EL_NetworkUtils.IsMaster(owner);
		if (m_bIsMaster)
			Rpc_DoLoadBankAccount();
		else
			Rpc(Rpc_DoLoadBankAccount);
	}
	
	//------------------------------------------------------------------------------------------------	
	override void EOnInit(IEntity owner)
	{
		GetGame().GetCallqueue().CallLater(LateInit, 100, false, owner);
		m_BankManager = EL_GlobalBankAccountManager.GetInstance();
	}
	
	//------------------------------------------------------------------------------------------------	
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
}

