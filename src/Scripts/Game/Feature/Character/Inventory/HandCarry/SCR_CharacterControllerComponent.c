modded class SCR_CharacterControllerComponent
{
	//------------------------------------------------------------------------------------------------
	override void OnGadgetStateChanged(IEntity gadget, bool isInHand, bool isOnGround)
	{
		super.OnGadgetStateChanged(gadget, isInHand, isOnGround);
		EL_HandInventoryStorageComponent handStorage = EL_ComponentFinder<EL_HandInventoryStorageComponent>.Find(GetOwner());
		if (handStorage) handStorage.OnGadgetStateChanged(gadget, isInHand, isOnGround);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_OnWeaponChanged(BaseWeaponComponent weapon, BaseWeaponComponent prevWeapon)
	{
		EL_HandInventoryStorageComponent handStorage = EL_ComponentFinder<EL_HandInventoryStorageComponent>.Find(GetOwner());
		if (handStorage) handStorage.OnWeaponChanged(weapon, prevWeapon);
	}

	//------------------------------------------------------------------------------------------------
	override void OnControlledByPlayer(IEntity owner, bool controlled)
	{
		super.OnControlledByPlayer(owner, controlled);

		// We need the eventhandler only locally on the players machine
		PlayerController playerController = GetGame().GetPlayerController();
		if (!controlled || !playerController || owner != playerController.GetControlledEntity()) return;

		EventHandlerManagerComponent eventHandler = EL_ComponentFinder<EventHandlerManagerComponent>.Find(owner);
		if (eventHandler) eventHandler.RegisterScriptHandler("OnWeaponChanged", this, EL_OnWeaponChanged, false);
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_CharacterControllerComponent()
	{
		EventHandlerManagerComponent eventHandler = EL_ComponentFinder<EventHandlerManagerComponent>.Find(GetOwner());
		if (eventHandler) eventHandler.RemoveScriptHandler("OnWeaponChanged", this, EL_OnWeaponChanged, false);
	}
}
