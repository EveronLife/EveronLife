modded class SCR_InventoryStorageLootUI : SCR_InventoryStorageBaseUI {
	
	override void GetAllItems( out notnull array<IEntity> pItemsInStorage, BaseInventoryStorageComponent pStorage = null )
	{
		
		//Create items array
		array<IEntity> superItems = new array<IEntity>();
		
		//Call base method
		super.GetAllItems(superItems, pStorage);
		
		
		//Get the users entity
		IEntity playerEntity = GetGame().GetPlayerController().GetControlledEntity();
		
		//Check items in storage for visibility
		foreach(IEntity item : superItems) {
		
			
			//Get the items access control component
			EL_InventoryStorageAccessControl accessControl = EL_InventoryStorageAccessControl.Cast(item.FindComponent(EL_InventoryStorageAccessControl));
		
			//Check if the item has an access control component
			if (accessControl) {
			
				//Check if its locked
				if (accessControl.IsLocked(playerEntity))
					continue;
				
			}
			
			//Add the item to the array
			pItemsInStorage.Insert(item);
		
		}
	}

}