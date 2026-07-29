modded class SCR_OpenVehicleDoorUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;

		if (!EL_VehicleLockComponent.CheckCanInteract(GetOwner(), user))
		{
			SetCannotPerformReason("Locked");
			return false;
		}

		return true;
	}
}
