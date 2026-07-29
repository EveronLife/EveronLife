class EL_VehicleLockAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		EL_VehicleLockComponent vehicleLock = EL_VehicleLockComponent.Cast(pOwnerEntity.FindComponent(EL_VehicleLockComponent));
		vehicleLock.ToggleLocked();

		if (vehicleLock.IsVehicleLocked())
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Your vehicle is now locked.", "Vehicle Locked", 3);
		else
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Your vehicle is now unlocked.", "Vehicle Unlocked", 3);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Lock/Unlock Vehicle";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		EL_VehicleLockComponent vehicleLock = EL_VehicleLockComponent.Cast(GetOwner().FindComponent(EL_VehicleLockComponent));

		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
		{
			SetCannotPerformReason("Missing Key");
			return false;
		}

		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
		if (characterController && vehicleLock.IsValidKey(characterController.GetAttachedGadgetAtLeftHandSlot()))
			return true;

		array<IEntity> inventoryItems();
		inventoryManager.GetItems(inventoryItems);

		foreach (IEntity item : inventoryItems)
		{
			if (vehicleLock.IsValidKey(item))
				return true;
		}

		SetCannotPerformReason("Missing Key");
		return false;
	}
}
