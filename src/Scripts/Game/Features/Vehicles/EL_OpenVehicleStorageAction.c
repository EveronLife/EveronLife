modded class SCR_OpenVehicleStorageAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;

		return EL_VehicleLockComponent.CheckCanInteract(m_Vehicle, user);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;

		if (!EL_VehicleLockComponent.CheckCanInteract(m_Vehicle, user))
		{
			SetCannotPerformReason("Locked");
			return false;
		}

		return true;
	}
}
