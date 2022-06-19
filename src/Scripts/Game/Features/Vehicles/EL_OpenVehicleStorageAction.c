class EL_OpenVehicleStorageAction : SCR_OpenVehicleStorageAction {
	
	
	override bool CanBePerformedScript(IEntity user)
 	{
		
		//Call base method
		if (!super.CanBePerformedScript(user))
			return false;
		
		
		//Get the vehicle's lock
		EL_VehicleLockComponent vehicleLock = EL_VehicleLockComponent.Cast(this.GetOwner().FindComponent(EL_VehicleLockComponent));
		
		//If the vehicle isnt locked, then yes we can perform!
		if (!vehicleLock.m_IsLocked)
			return true;
		
		//Get the users inventory
		auto inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		
		//Get the users IEntity
		array<IEntity> inventoryItems = new array<IEntity>();
		inventoryManager.GetItems(inventoryItems);
		
		
		//Loop through inventory items
		foreach (IEntity item : inventoryItems)
		{
			
			//Read the inventory item
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (pInvComp)
			{

				//Check if its a valid key
				if (vehicleLock.IsValidKey(item))
					return true;
			}
		}
		
		//Set the cant be feformed reason
		SetCannotPerformReason("Locked");
		
		return false;
 	}
	
}