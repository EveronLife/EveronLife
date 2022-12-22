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
		RplComponent vehicleShopRpl = RplComponent.Cast(Replication.FindItem(shopId));
		EL_VehicleShopManagerComponent vehicleShopManager = EL_ComponentFinder<EL_VehicleShopManagerComponent>.Find(vehicleShopRpl.GetEntity());
		string playerUID = EL_Utils.GetPlayerUID(GetOwner());
		
		vehicleShopManager.DoBuyVehicle(vehiclePrefab, color, playerUID);
	}
	
	//------------------------------------------------------------------------------------------------	
	void AskLoadGarage(IEntity garageEnt)
	{
		RplComponent rplComp = EL_ComponentFinder<RplComponent>.Find(garageEnt);
		Rpc(Rpc_AskLoadGarage, rplComp.Id());
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_AskLoadGarage(RplId garageId)
	{
		RplComponent garageRplComp = RplComponent.Cast(Replication.FindItem(garageId));
		EL_GarageManagerComponent garageManager = EL_ComponentFinder<EL_GarageManagerComponent>.Find(garageRplComp.GetEntity());
		
		garageManager.DoLoadGarage(GetOwner());
	}
}
