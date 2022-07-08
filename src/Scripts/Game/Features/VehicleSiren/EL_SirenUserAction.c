class EL_SirenUserAction: SCR_VehicleActionBase
{

	protected EL_SirenControllerComponent sirenController;
	SoundComponent soundComponent;
	AudioHandle audioHandle;
	
	protected bool state = false;
	
	void ToggleSiren()
	{
		if (state == false) {
			audioHandle = soundComponent.SoundEvent("SOUND_SIREN");
			state = true;
		} else {
			soundComponent.Terminate(audioHandle);
			state = false;
		}
	}
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		sirenController = EL_SirenControllerComponent.Cast(pOwnerEntity.FindComponent(EL_SirenControllerComponent));
		soundComponent = SoundComponent.Cast(pOwnerEntity.FindComponent(SoundComponent));
	}
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{		
		ToggleSiren();
	}
	
}
