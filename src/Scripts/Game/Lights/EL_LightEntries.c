[BaseContainerProps()]
class EL_BaseEntry
{
	protected ref array<EL_LightComponent> m_AffectedLights = {};
	
	void OnRegister(EL_LightComponent light);
	
	void OnExecute(EL_LightAnimation animation);
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleField("m_DelayMS", "Wait %1ms")]
class EL_WaitEntry : EL_BaseEntry
{
	[Attribute(desc: "Time in milliseconds that the entry will take", params: "0")]
	protected int m_DelayMS;
	
	override void OnExecute(EL_LightAnimation animation)
	{
		animation.SetDelay(m_DelayMS / 1000.0);
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleField("m_Steps", "Loop %1 steps")]
class EL_LoopEntry : EL_BaseEntry
{
	[Attribute(desc: "Number of steps to loop. If 0, the animation will loop from the beginning")]
	protected int m_Steps;
	
	[Attribute(desc: "Number of times the loop should be executed. Number 0 means infinite")]
	protected int m_LoopCount;
	
	protected int m_LoopsRemaining = m_LoopCount;
	
	override void OnExecute(EL_LightAnimation animation)
	{
		if(m_Steps)
		{
			if(m_LoopsRemaining > 0 || !m_LoopCount)
			{
				if(m_LoopCount)
					m_LoopsRemaining--;
				animation.GoBack(m_Steps + 1);
			}
			else   
			{
				m_LoopsRemaining = m_LoopCount;
			}
		}
		else
		{	
			animation.Reset();
		}
	
	}
}

enum EL_LightEntryType
{
	TURN_LIGHTS_ON,
	TURN_LIGHTS_OFF,
	TOGGLE_LIGHTS
}

[BaseContainerProps(), EL_BaseContainerCustomTitleEnumReadable(EL_LightEntryType, "m_Type")]
class EL_LightEntry : EL_WaitEntry
{
	
	
	protected ref static const ParamEnumArray enums = 
	{
		ParamEnum("Turn On", EL_LightEntryType.TURN_LIGHTS_ON.ToString()),
		ParamEnum("Turn Off", EL_LightEntryType.TURN_LIGHTS_OFF.ToString()),
		ParamEnum("Toggle", EL_LightEntryType.TOGGLE_LIGHTS.ToString())
	};
	
	[Attribute("Turn On", uiwidget: UIWidgets.ComboBox, enums: enums)]
	protected EL_LightEntryType m_Type;

	[Attribute(desc: "Names separated by space. Lights that contains the name will be affected by the entry. If left blank all lights are affected.")]
	protected string m_Name;
	
	protected ref array<EL_LightComponent> m_Lights = {};
	
	override void OnRegister(EL_LightComponent light)
	{
		if(Affects(light)) m_Lights.Insert(light);
	}
	
	override void OnExecute(EL_LightAnimation animation)
	{
		foreach(EL_LightComponent light : m_Lights)
		{
			Act(light);
		}
		animation.SetDelay(m_DelayMS / 1000.0);
	}
	
	bool Affects(EL_LightComponent light)
	{
		if(!m_Name) return true;
		array<string> names = {};
		m_Name.Split(" ", names, true);
		foreach(string name : names)
		{
			if(light.GetName().Contains(name))
				return true;
		}
		
		return false;
	}
	
	void Act(EL_LightComponent light)
	{
		if(m_Type == EL_LightEntryType.TURN_LIGHTS_ON) light.TurnOn();
		else if(m_Type == EL_LightEntryType.TURN_LIGHTS_OFF) light.TurnOff();
		else light.Toggle();
	}
}

enum EL_AnimationEntryType
{
	TURN_ANIMATION_ON,
	TURN_ANIMATION_OFF,
	TOGGLE_ANIMATION
}


[BaseContainerProps(), EL_BaseContainerCustomTitleEnumReadable(EL_AnimationEntryType, "m_Type")]
class EL_AnimationEntry : EL_LightEntry
{
	override void Act(EL_LightComponent light)
	{
		if(m_Type == EL_AnimationEntryType.TURN_ANIMATION_ON) light.Play();
		else if(m_Type == EL_AnimationEntryType.TURN_ANIMATION_OFF) light.Stop();
		else light.ToggleAnim();
	}
}
