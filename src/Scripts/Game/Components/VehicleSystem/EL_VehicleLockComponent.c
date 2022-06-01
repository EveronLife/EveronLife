class EL_VehicleLockComponentClass : SCR_BaseLockComponentClass {

}

class EL_VehicleLockComponent : SCR_BaseLockComponent {
	
	[Attribute("Debug Identifier")]
	string m_DebugIdentifier;
	
	[RplProp()]
	string m_VehicleIdentifier;
	
	[RplProp()]
	bool m_IsLocked = true;
	
	
	void EL_VehicleLockComponent() {
		if (m_DebugIdentifier != "")
			m_VehicleIdentifier = m_DebugIdentifier;
	}
	
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
			//Check if its a valid key
			if (IsValidKey(item))
				return false;
		}
		
		return true;
	}
	
	protected bool IsValidKey(IEntity key) {
	
		//Get the key component
		EL_VehicleKeyComponent keyComponent = EL_VehicleKeyComponent.Cast(key.FindComponent(EL_VehicleKeyComponent));
		
		//Check that there is a key component
		if (!keyComponent)
			return false;
		
		//Check the keys identifier
		if (keyComponent.m_VehicleIdentifier == m_VehicleIdentifier)
			return true;
	
		return false;
	}
	
	override LocalizedString GetCannotPerformReason(IEntity user)
	{
		
		//Check if locked
		if (m_IsLocked)
			return "Locked";
		
		return super.GetCannotPerformReason(user);
	}

}