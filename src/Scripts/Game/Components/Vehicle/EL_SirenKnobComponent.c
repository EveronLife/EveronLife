class EL_SirenKnobComponentClass : ScriptComponentClass
{

}

class EL_SirenKnobComponent : ScriptComponent
{
	protected SoundComponent m_SoundComp;
	
	override void OnPostInit(IEntity owner)
	{
		m_SoundComp = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
		Register(owner);
	}
	
	void Register(IEntity owner)
	{
		IEntity parent = owner.GetParent();
		EL_SirenManagerComponent manager;
		
		while(parent && !manager)
		{
			manager = EL_SirenManagerComponent.Cast(parent.FindComponent(EL_SirenManagerComponent));
			parent = parent.GetParent();
		}
		if(manager) manager.RegisterKnob(SignalsManagerComponent.Cast(owner.FindComponent(SignalsManagerComponent)));
		else Print("Siren knob component with no manager", LogLevel.WARNING);
	}
}