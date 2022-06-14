class EL_SirenUserAction: SCR_VehicleActionBase
{

	protected EL_SirenControllerComponent sirenController;
	SoundComponent soundComponent;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		sirenController = EL_SirenControllerComponent.Cast(pOwnerEntity.FindComponent(EL_SirenControllerComponent));
		soundComponent = SoundComponent.Cast(pOwnerEntity.FindComponent(SoundComponent));
	}
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{		
		Print("BOOL: " + sirenController.IsSirenOn());
		if(GetState())
		{
			SetState(false);
			soundComponent.TerminateAll();
		}else
		{
			SetState(true);
			soundComponent.SoundEvent("SOUND_SIREN");
		}
		
	}
	
	override bool GetState()
	{
		return sirenController && sirenController.IsSirenOn();
	}
	
	override void SetState(bool enable)
	{
		if (!sirenController)
			return;
		
		if (enable)
			sirenController.IsSirenOn() = true;
		else
			sirenController.IsSirenOn() = false;
	}
}
