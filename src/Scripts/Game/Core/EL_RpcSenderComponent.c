[EntityEditorProps(category: "EveronLife/Core")]
class EL_RpcSenderComponentClass : ScriptComponentClass
{
}

class EL_RpcSenderComponent  : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	void AskWithdrawFromBankAccount()
	{
		Rpc(Rpc_DoWithdrawFromBankAccount);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_DoWithdrawFromBankAccount()
	{
		Print("[EL-Bank] 2.1 Server got load reques for " + GetOwner());
		EL_GlobalBankAccountManager.GetInstance().LoadPlayerBankAccount(GetOwner(), GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetOwner()));
	}

	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_DoLoadBankAccount()
	{
		Print("[EL-Bank] 2.1 Server got load reques for " + GetOwner());
		EL_GlobalBankAccountManager.GetInstance().LoadPlayerBankAccount(GetOwner(), GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Init after Replication
	void LateInit(IEntity owner)
	{
		if (EL_NetworkUtils.IsMaster(owner))
			Rpc_DoLoadBankAccount();
		else
			Rpc(Rpc_DoLoadBankAccount);
	}
	
	//------------------------------------------------------------------------------------------------	
	override void EOnInit(IEntity owner)
	{
		GetGame().GetCallqueue().CallLater(LateInit, 100, false, owner);
	}
	
	//------------------------------------------------------------------------------------------------	
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
}

