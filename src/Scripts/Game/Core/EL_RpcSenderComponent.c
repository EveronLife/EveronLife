[EntityEditorProps(category: "EveronLife/Core")]
class EL_RpcSenderComponentClass : ScriptComponentClass
{
}

class EL_RpcSenderComponent  : ScriptComponent
{
	//------------------------------------------------------------------------------------------------	
	void AskLoadBankAccount()
	{
		Rpc(Rpc_DoLoadBankAccount);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_DoLoadBankAccount()
	{
		EL_GlobalBankAccountManager.GetInstance().LoadPlayerBankAccount(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetOwner()));
	}
}
