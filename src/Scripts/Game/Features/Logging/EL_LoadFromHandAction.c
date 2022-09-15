class EL_LoadFromHandAction : ScriptedUserAction
{

	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(pUserEntity.FindComponent(CharacterControllerComponent));
		IEntity handItem = characterController.GetAttachedGadgetAtLeftHandSlot();
		EL_ForcedHandGadgetComponent gadgetComponent = EL_ForcedHandGadgetComponent.Cast(handItem.FindComponent(EL_ForcedHandGadgetComponent));
		SCR_VehicleInventoryStorageManagerComponent inventoryManager = SCR_VehicleInventoryStorageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		inventoryManager.TryMoveItemToStorage(handItem, inventoryManager.FindStorageForItem(handItem, EStoragePurpose.PURPOSE_ANY ));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Load in Vehicle");
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
		IEntity handItem = characterController.GetAttachedGadgetAtLeftHandSlot();
		if (!handItem)
			return false;
		SCR_VehicleInventoryStorageManagerComponent inventoryManager = SCR_VehicleInventoryStorageManagerComponent.Cast(GetOwner().FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;
		return (inventoryManager.FindStorageForItem(handItem, EStoragePurpose.PURPOSE_ANY)));
 	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Can't store item");
	}

}
