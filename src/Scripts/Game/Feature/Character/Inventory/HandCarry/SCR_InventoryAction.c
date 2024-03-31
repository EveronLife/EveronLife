modded class SCR_InventoryAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(user);
		if (characterController && EL_Component<EL_HandCarryComponent>.Find(characterController.GetAttachedGadgetAtLeftHandSlot()))
		{
			//SetCannotPerformReason("Hands obstructed"); // TODO localize and uncomment then
			return false;
		}

		return super.CanBePerformedScript(user);
	}
}
