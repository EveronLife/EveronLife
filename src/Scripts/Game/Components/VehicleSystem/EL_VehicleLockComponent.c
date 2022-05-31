class EL_VehicleLockComponentClass : SCR_BaseLockComponentClass {

}

class EL_VehicleLockComponent : SCR_BaseLockComponent {
	
	[RplProp()]
	string m_VehicleIdentifier = "123456";
	
	[RplProp()]
	bool m_IsLocked = true;
	
	override bool IsLocked(IEntity user, BaseCompartmentSlot compartmentSlot)
	{
		
		//Call base
		if (compartmentSlot)
			if (super.IsLocked(user, compartmentSlot))
				return true;
		
		//If the vehicle is unlocked than anyone can enter
		if (!m_IsLocked)
			return false;
		
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
				if(!attribs )
					break;
			
				
				//Get the keys key component
				auto keyComponent = EL_VehicleKeyComponent.Cast(item.FindComponent(EL_VehicleKeyComponent));
				
				//Check if the key is valid
				if (keyComponent) 
					if (keyComponent.m_VehicleIdentifier == m_VehicleIdentifier)
							return false;
			}
		}
		
		return true;
	}
	
	override LocalizedString GetCannotPerformReason(IEntity user)
	{
		
		//Check if locked
		if (m_IsLocked)
			return "Locked";
		
		return super.GetCannotPerformReason(user);
	}

}