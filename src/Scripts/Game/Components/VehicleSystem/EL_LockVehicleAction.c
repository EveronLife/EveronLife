class EL_VehicleLockAction : ScriptedUserAction
{
	
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		
		//Get the vehicles locking class
		auto vehicleLock = EL_VehicleLockComponent.Cast(pOwnerEntity.FindComponent(EL_VehicleLockComponent));
		
		//Check for lock
		//if (!vehicleLock)
		//	return;
		
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
		//Get the users inventory
		auto inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		
		//Get the users IEntity
		array<IEntity> inventoryItems = new array<IEntity>();
		inventoryManager.GetItems(inventoryItems);
		
		
		//Loop through items
		foreach (IEntity item : inventoryItems)
		{
			
			//Read the inventory item
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (pInvComp)
			{
				
				//Make sure there is item attributes
				ItemAttributeCollection attribs = pInvComp.GetAttributes();
				if( !attribs )
					break;
			
				
				//Get the keys key component
				auto keyComponent = EL_VehicleKeyComponent.Cast(item.FindComponent(EL_VehicleKeyComponent));
				
				//Check if the key is valid
				if (keyComponent) 
					if (keyComponent.m_VehicleIdentifier == EL_VehicleLockComponent.Cast(this.GetOwner().FindComponent(EL_VehicleLockComponent)).m_VehicleIdentifier)
							return true;
			}
		}
		
		//Set the cant be feformed reason
		SetCannotPerformReason("Missing Key");
		
		return false;
 	}
	
	
}
