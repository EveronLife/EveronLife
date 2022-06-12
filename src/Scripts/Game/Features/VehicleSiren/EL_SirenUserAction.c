class EL_SirenUserAction: ScriptedUserAction
{

	//! Should action behave as a toggle
	[Attribute( uiwidget: UIWidgets.CheckBox, defvalue: "1", desc: "Should action behave as a toggle")]
	protected bool m_bIsToggle;

	//! Target state of the action, ignored if toggle
	[Attribute( uiwidget: UIWidgets.CheckBox, defvalue: "1", desc: "Target state of the action, ignored if toggle")]
	protected bool m_bTargetState;

	//! Description of action to toggle on
	[Attribute( uiwidget: UIWidgets.Auto, defvalue: "#AR-UserAction_State_On", desc: "Description of action to toggle on")]
	private string m_sActionStateOn;

	//! Description of action to toggle off
	[Attribute( uiwidget: UIWidgets.Auto, defvalue: "#AR-UserAction_State_Off", desc: "Description of action to toggle off")]
	private string m_sActionStateOff;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{		
		// Play sound
		SoundComponent soundComponent = SoundComponent.Cast(pOwnerEntity.FindComponent(SoundComponent));
		Print(soundComponent.IsPlaying());
		if(!soundComponent.IsPlaying())
		{
			soundComponent.SoundEvent("SOUND_SIREN");
		}else
		{
			//soundComponent.SoundEvent("SOUND_SIREN_STOP");
		}
	}
}
