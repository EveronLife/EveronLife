class EL_ForceEquipAction : ScriptedUserAction
{

	
	//------------------------------------------------------------------------------------------------

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(pUserEntity.FindComponent(CharacterControllerComponent));
		characterController.TakeGadgetInLeftHand(pOwnerEntity, EGadgetType.CONSUMABLE);
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
		return (characterController.CanEquipGadget(GetOwner()));
 	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Can't carry item");
	}

}
