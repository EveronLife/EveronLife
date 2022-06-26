class EL_LightSlot : BaseLightSlot 
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.EditBox, desc: "HitZone", category: "Damage" )]
	protected string m_sHitZoneName;
	[Attribute(defvalue: "", uiwidget: UIWidgets.EditBox, desc: "Name of the sound event that should be called when the lights are activated.", category: "Sounds" )]
	protected string m_sLightOnEventName;
	[Attribute(defvalue: "", uiwidget: UIWidgets.EditBox, desc: "Name of the sound event that should be called when the lights are activated.", category: "Sounds" )]
	protected string m_sLightOffEventName;
	
	protected bool m_bIsActive;
	protected SoundComponent m_SoundComponent;

	private ref ScriptInvoker Event_EOnLightStateChanged = new ref ScriptInvoker();
	
	/*!
	Get event called when the light is turned on or off.
	\return Script invoker
	*/
	ref notnull ScriptInvoker GetOnLightStateChanged()
	{
		return Event_EOnLightStateChanged;
	}
	
	override void OnLightStateChanged(bool newState)
	{
		if (m_bIsActive == newState)
			return;
		
		m_bIsActive = newState;
		Event_EOnLightStateChanged.Invoke(newState);
	}
	
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		m_SoundComponent = SoundComponent.Cast(owner.FindComponent(SoundComponent));
		
		if (m_SoundComponent)
			Event_EOnLightStateChanged.Insert(PlaySound);
	}
	
	void PlaySound(bool newState)
	{
		string eventName;
		
		if (newState)
			eventName = m_sLightOnEventName;
		else
			eventName = m_sLightOffEventName;
		
		if (m_SoundComponent && !eventName.IsEmpty())
			m_SoundComponent.SoundEvent(eventName);	
	}
	
	string GetHitZoneName()
	{
		return m_sHitZoneName;
	}
};