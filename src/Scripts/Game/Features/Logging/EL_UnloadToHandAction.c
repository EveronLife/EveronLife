class EL_UnloadToHandAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		array<typename> componentsQuery = {EL_ForcedHandGadgetComponent};
		SCR_VehicleInventoryStorageManagerComponent vehicleInventoryManager = SCR_VehicleInventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		
		IEntity gadgetInInv = vehicleInventoryManager.FindItemWithComponents(componentsQuery, EStoragePurpose.PURPOSE_ANY);
		SCR_InventoryStorageManagerComponent playerInventoryManager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		playerInventoryManager.TryMoveItemToStorage(gadgetInInv, playerInventoryManager.FindStorageForItem(gadgetInInv, EStoragePurpose.PURPOSE_ANY ));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Unload from Vehicle");
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		SCR_VehicleInventoryStorageManagerComponent vehicleInventoryManager = SCR_VehicleInventoryStorageManagerComponent.Cast(GetOwner().FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		SCR_InventoryStorageManagerComponent playerInventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!playerInventoryManager)
			return false;
		
		array<typename> componentsQuery = {EL_ForcedHandGadgetComponent};
		IEntity gadgetInInv = vehicleInventoryManager.FindItemWithComponents(componentsQuery, EStoragePurpose.PURPOSE_ANY);
		return (playerInventoryManager.FindStorageForItem(gadgetInInv, EStoragePurpose.PURPOSE_ANY)));
 	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Can't carry item");
	}

}
