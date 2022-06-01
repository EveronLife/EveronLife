class EL_VehicleLockAction : ScriptedUserAction
{
	
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		
		//Get the vehicles locking class
		auto vehicleLock = EL_VehicleLockComponent.Cast(pOwnerEntity.FindComponent(EL_VehicleLockComponent));
		
		//Swap islocked
		vehicleLock.m_IsLocked = !vehicleLock.m_IsLocked;
		
		//Hints
		if (vehicleLock.m_IsLocked) {
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Your vehicle is now locked.", "Vehicle Locked", 3);
		} else {
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Your vehicle is now unlocked.", "Vehicle Unlocked", 3);
		};
	}
	
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Lock/Unlock Vehicle");
		return true;
	}
	
	override bool CanBePerformedScript(IEntity user)
 	{
		
		//Get the vehicle's lock
		EL_VehicleLockComponent vehicleLock = EL_VehicleLockComponent.Cast(this.GetOwner().FindComponent(EL_VehicleLockComponent));
		
		
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

				//Get the keys key component
				auto keyComponent = EL_VehicleKeyComponent.Cast(item.FindComponent(EL_VehicleKeyComponent));
				
				//Check if the key is valid
				if (keyComponent) {
				
					//Check if the identifier matches
					if (keyComponent.m_VehicleIdentifier == vehicleLock.m_VehicleIdentifier)
							return true;
				}
			}
		}
		
		//Set the cant be feformed reason
		SetCannotPerformReason("Missing Key");
		
		return false;
 	}
	
	
}
