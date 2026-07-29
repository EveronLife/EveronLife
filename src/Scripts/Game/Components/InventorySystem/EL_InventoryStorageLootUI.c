modded class SCR_InventoryStorageLootUI : SCR_InventoryStorageBaseUI {

	override void GetAllItems( out notnull array<IEntity> pItemsInStorage, BaseInventoryStorageComponent pStorage = null )
	{
		IEntity playerEntity = GetGame().GetPlayerController().GetControlledEntity();

		if (pStorage && playerEntity)
		{
			Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(pStorage.GetOwner(), true));
			if (vehicle)
			{
				EL_VehicleLockComponent vehicleLock = EL_VehicleLockComponent.Cast(vehicle.FindComponent(EL_VehicleLockComponent));
				if (vehicleLock && vehicleLock.IsVehicleLocked())
					return;
			}
		}

		array<IEntity> superItems = new array<IEntity>();
		super.GetAllItems(superItems, pStorage);

		foreach(IEntity item : superItems) {
			EL_BaseInventoryStorageAccessControl accessControl = EL_BaseInventoryStorageAccessControl.Cast(item.FindComponent(EL_BaseInventoryStorageAccessControl));
			if (accessControl && accessControl.IsLocked(playerEntity))
				continue;
			pItemsInStorage.Insert(item);
		}
	}

}