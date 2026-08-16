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
		//Custom string encoder
		array<string> transactionComments = {};
		foreach (EL_BankTransaction transaction : account.m_aTransactions)
		{
			transactionComments.Insert(transaction.m_sComment);
		}
		Rpc(Rpc_DoSetLocalBankAccount, account, transactionComments);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void Rpc_DoSetLocalBankAccount(EL_BankAccount account, array<string> transactionComments)
	{
		//Custom string decoder
		foreach (int i, string comment : transactionComments)
		{
			account.m_aTransactions[i].m_sComment = comment;
		}

		m_BankManager.m_LocalBankAccount = account;
		//Update open bank ui
		EL_BankMenu bankMenu = EL_BankMenu.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.EL_BankMenu));
		if (bankMenu)
			bankMenu.OnMenuFocusGained();
	}

	//------------------------------------------------------------------------------------------------
	void AskTransactionFromBankAccount(int amount, string comment)
	{
		if (m_bIsMaster)
			Rpc_DoTransactionBankAccount(amount, comment);
		else
			Rpc(Rpc_DoTransactionBankAccount, amount, comment);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_DoTransactionBankAccount(int amount, string comment)
	{
		EL_BankAccount updatedAccount = m_BankManager.NewAccountTransaction(EL_Utils.GetPlayerUID(GetOwner()), amount, comment);

		//Send updated account back to client
		AskSetLocalBankAccount(updatedAccount);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_DoLoadBankAccount()
	{
		m_BankManager.LoadPlayerBankAccount(GetOwner());
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
