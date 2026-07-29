 modded class SCR_CloseVehicleDoorUserAction
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
 	//------------------------------------------------------------------------------------------------
 	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 	{
 		if (!pOwnerEntity || !pUserEntity)
 			return;
 		super.PerformAction(pOwnerEntity, pUserEntity);
 	}
 }
