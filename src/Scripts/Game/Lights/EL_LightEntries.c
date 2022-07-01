[BaseContainerProps()]
class EL_BaseEntry : ScriptAndConfig
{
	protected ref array<EL_LightComponent> m_AffectedLights = {};
	
	void OnRegister(EL_LightComponent light);
	
	void OnExecute(EL_LightAnimation animation);
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleField("m_DelayMS", "%1ms")]
class EL_WaitEntry : EL_BaseEntry
{
	[Attribute()]
	protected int m_DelayMS;
	
	override void OnExecute(EL_LightAnimation animation)
	{
		animation.SetDelay(m_DelayMS / 1000.0);
	}
}

class EL_LoopEntry : EL_BaseEntry
{
	[Attribute()]
	protected int m_Steps;
	
	[Attribute()]
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

enum EL_EntryType
{
	TURN_ON,
	TURN_OFF,
	TOGGLE
}

class EL_LightEntry : EL_WaitEntry
{
	protected ref static const ParamEnumArray enums = 
	{
		ParamEnum("Turn On", EL_EntryType.TURN_ON.ToString()),
		ParamEnum("Turn Off", EL_EntryType.TURN_OFF.ToString()),
		ParamEnum("Toggle", EL_EntryType.TOGGLE.ToString())
	};
	
	[Attribute(defvalue: EL_EntryType.TURN_ON.ToString(), uiwidget: UIWidgets.ComboBox, enums: enums)]
	protected EL_EntryType m_Type;

	[Attribute()]
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
		if(m_Type == EL_EntryType.TURN_ON) light.TurnOn();
		else if(m_Type == EL_EntryType.TURN_OFF) light.TurnOff();
		else light.Toggle();
	}
}

class EL_AnimateEntry : EL_LightEntry
{
	override void Act(EL_LightComponent light)
	{
		if(m_Type == EL_EntryType.TURN_ON) light.Play();
		else if(m_Type == EL_EntryType.TURN_OFF) light.Stop();
		else light.ToggleAnim();
	}
}
