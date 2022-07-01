class EL_SirenKnobComponentClass : ScriptComponentClass
{

}

class EL_SirenKnobComponent : ScriptComponent
{
	protected SoundComponent m_SoundComp;
	
	protected EL_LightAnimation m_Animation;
	
	override void OnPostInit(IEntity owner)
	{
		m_SoundComp = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
		SetEventMask(owner, EntityEvent.SIMULATE);
		Register();
	}
	
	void Register()
	{
		IEntity parent = GetOwner().GetParent();
		EL_SirenManagerComponent manager;
		
		while(parent && !manager)
		{
			manager = EL_SirenManagerComponent.Cast(parent.FindComponent(EL_SirenManagerComponent));
			parent = parent.GetParent();
		}
		if(manager) manager.RegisterKnob(this);
		else Print("Siren knob component with no manager", LogLevel.WARNING);
	}
	
	void SetAnimation(EL_LightAnimation anim)
	{
		m_Animation = anim;
	}
	
	override event protected void EOnSimulate(IEntity owner, float timeSlice)
	{
		if(m_Animation)
			m_Animation.Tick(timeSlice);
	}
}