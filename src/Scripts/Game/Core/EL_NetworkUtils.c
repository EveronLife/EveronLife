class EL_NetworkUtils : EPF_NetworkUtils
{
	//------------------------------------------------------------------------------------------------
	static EL_RpcSenderComponent GetLocalRpcSender()
	{
		return EPF_Component<EL_RpcSenderComponent>.Find(SCR_PlayerController.GetLocalControlledEntity());
	}
	//------------------------------------------------------------------------------------------------
	static bool IsMaster(IEntity entity)
	{
		if (!entity) return false;
		RplComponent replication = RplComponent.Cast(entity.FindComponent(RplComponent));
		if (!replication) return false;
		return replication.IsMaster();
	}
}
