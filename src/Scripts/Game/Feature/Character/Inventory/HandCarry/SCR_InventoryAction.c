modded class SCR_InventoryAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		CharacterControllerComponent characterController = EL_ComponentFinder<CharacterControllerComponent>.Find(user);
		if (characterController && EL_ComponentFinder<EL_HandCarryComponent>.Find(characterController.GetAttachedGadgetAtLeftHandSlot()))
		{
			//SetCannotPerformReason("Hands obstructed"); // TODO localize and uncomment then
			return false;
		}

		return super.CanBePerformedScript(user);
	}
}
