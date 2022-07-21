class EL_ForceEquipAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	private bool TryHolsterWeapon(IEntity pUserEntity)
	{
		BaseWeaponManagerComponent weaponManager = BaseWeaponManagerComponent.Cast(pUserEntity.FindComponent(BaseWeaponManagerComponent));
		SCR_InventoryStorageManagerComponent playerInventory = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));

		BaseWeaponComponent weapon = weaponManager.GetCurrent();
		if (!weapon)
			return true;
		WeaponSlotComponent weaponSlot = WeaponSlotComponent.Cast(weapon);
		IEntity weaponEntity = weaponSlot.GetWeaponEntity();
		
		return (playerInventory.TryMoveItemToStorage(weaponEntity, playerInventory.FindStorageForItem(weaponEntity)));
	}

	//------------------------------------------------------------------------------------------------
	private bool CanHolsterWeapon(IEntity pUserEntity)
	{
		BaseWeaponManagerComponent weaponManager = BaseWeaponManagerComponent.Cast(pUserEntity.FindComponent(BaseWeaponManagerComponent));
		SCR_InventoryStorageManagerComponent playerInventory = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));

		BaseWeaponComponent weapon = weaponManager.GetCurrent();
		if (!weapon)
			return true;
		WeaponSlotComponent weaponSlot = WeaponSlotComponent.Cast(weapon);
		IEntity weaponEntity = weaponSlot.GetWeaponEntity();
		
		
		return (playerInventory.CanMoveItemToStorage(weaponEntity, playerInventory.FindStorageForItem(weaponEntity)));
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(pUserEntity.FindComponent(CharacterControllerComponent));

		//Remove any item in hand
		characterController.RemoveGadgetFromHand();
		characterController.TakeGadgetInLeftHand(pOwnerEntity, EGadgetType.CONSUMABLE);
		if (!TryHolsterWeapon(pUserEntity))
			characterController.RemoveGadgetFromHand();
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Pickup");
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
		if (!CanHolsterWeapon(user))
		{
			SetCannotPerformReason("No inventory space to holster weapon");
			return false;
		}

		return (characterController.CanEquipGadget(GetOwner()));
 	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Can't carry item");
	}

}
