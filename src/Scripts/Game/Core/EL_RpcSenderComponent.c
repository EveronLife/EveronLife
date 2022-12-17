[EntityEditorProps(category: "EveronLife/Core")]
class EL_RpcSenderComponentClass : ScriptComponentClass
{
}

class EL_RpcSenderComponent  : ScriptComponent
{
	
	//------------------------------------------------------------------------------------------------	
	void AskBuyVehicle(ResourceName vehiclePrefab, int color, IEntity vehicleShop)
	{
		RplComponent rplComp = EL_ComponentFinder<RplComponent>.Find(vehicleShop);
		Rpc(Rpc_AskBuyVehicle, vehiclePrefab, color, rplComp.Id());
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_AskBuyVehicle(ResourceName vehiclePrefab, int color, RplId shopId)
	{
		string playerUID = EL_Utils.GetPlayerUID(GetOwner());
		
		RplComponent vehicleShopRpl = RplComponent.Cast(Replication.FindItem(shopId));
		EL_VehicleShopManagerComponent vehicleShopManager = EL_VehicleShopManagerComponent.Cast(vehicleShopRpl.GetEntity().FindComponent(EL_VehicleShopManagerComponent));
		vehicleShopManager.DoBuyVehicle(vehiclePrefab, color, playerUID);
	}
}