class EL_SirenManagerComponentClass : ScriptComponentClass
{

}

class EL_SirenManagerComponent : ScriptComponent
{
	protected const float SOUND_OFF = 0;
	protected const float SOUND_ON = 1;
	
	[Attribute()]
	protected ref EL_SirenModes m_Modes;
	
	protected EL_SirenMode m_CurrentMode;
	
	protected ref EL_LightAnimation m_CurrentAnim;
	
	protected EL_SirenKnobComponent m_Knob;
	protected SignalsManagerComponent m_KnobSigComp;
	
	protected SoundComponent m_SoundComp;
	
	override void OnPostInit(IEntity owner)
	{
		m_SoundComp = SoundComponent.Cast(owner.FindComponent(SoundComponent));
		SetMode("default");
	}
	
	
	void RegisterLight(EL_LightComponent light)
	{
		if(m_Modes)
			m_Modes.Insert(light);
	}
	
	void RegisterKnob(EL_SirenKnobComponent knob)
	{
		if(m_Knob) Print("Siren with multiple knobs", LogLevel.WARNING);
		else 
		{
			m_Knob = knob;
			m_KnobSigComp = SignalsManagerComponent.Cast(knob.GetOwner().FindComponent(SignalsManagerComponent));
		}
	}
	
	protected void SetSirenMode(EL_SirenMode mode)
	{
		if(m_CurrentMode)
		{
			m_SoundComp.SetSignalValueStr(m_CurrentMode.GetSirenActive() + "Active", SOUND_OFF);
			m_SoundComp.SetSignalValueStr(m_CurrentMode.GetSirenInactive() + "Inactive", SOUND_OFF);
		}
		
		m_SoundComp.SetSignalValueStr(mode.GetSirenActive() + "Active", SOUND_ON);
		m_SoundComp.SetSignalValueStr(mode.GetSirenInactive() + "Inactive", SOUND_ON);
	}
	
	protected void SetKnobMode(EL_SirenMode mode)
	{
		EL_LightAnimation anim = mode.GetAnimation();
		if(anim)
		{
			anim.Reset();
			m_Knob.SetAnimation(anim);
		}
		foreach(SignalValuePair sig : mode.GetKnobSignals())
		{
			m_KnobSigComp.SetSignalValue(m_KnobSigComp.FindSignal(sig.GetName()), sig.GetValue());
		}
		
	}
	
	void SetMode(string name)
	{
		if(!m_Modes) return;
		EL_SirenMode mode = m_Modes.Find(name);
		if(mode)
		{
			if(m_Knob) SetKnobMode(mode);
			
			if(m_SoundComp) SetSirenMode(mode);
		}
		
		m_CurrentMode = mode;
	}
}

[BaseContainerProps(), SCR_BaseContainerCustomTitleField("m_Name", "%1")]
class EL_SirenMode
{
	static protected const ref ParamEnumArray SIREN_SOUNDS = 
	{
		new ParamEnum("None", "0"),
		new ParamEnum("DefaultHorn", "1"),
		new ParamEnum("SlowSiren", "2"),
		new ParamEnum("FastSiren", "3")
	};
	
	[Attribute()]
	protected string m_Name;
	
	[Attribute()]
	protected ref EL_LightAnimation m_Animation;
	
	[Attribute(uiwidget: UIWidgets.ComboBox, enums: SIREN_SOUNDS)]
	protected string m_SirenActive;
	
	[Attribute(uiwidget: UIWidgets.ComboBox, enums: SIREN_SOUNDS)]
	protected string m_SirenInactive;
	
	[Attribute()]
	protected ref array<ref SignalValuePair> m_KnobSignals;
	
	string GetName()
	{
		return m_Name;
	}
	
	
	string GetSirenActive()
	{
		return m_SirenActive;
	}
	
	string GetSirenInactive()
	{
		return m_SirenInactive;
	}
	
	EL_LightAnimation GetAnimation()
	{
	 	return m_Animation;
	}
	
	void Insert(EL_LightComponent light)
	{
		if(m_Animation) m_Animation.Insert(light);
	}
	
	array<ref SignalValuePair> GetKnobSignals()
	{
		return m_KnobSignals;
	}
}

[BaseContainerProps()]
class SignalValuePair
{
	[Attribute()]
	protected string m_Name;
	
	[Attribute()]
	protected float m_Value;
	
	string GetName()
	{
		return m_Name;
	}
	
	float GetValue()
	{
		return m_Value;
	}
}


[BaseContainerProps()]
class EL_SirenModes
{
	[Attribute()]
	protected ref array<ref EL_SirenMode> m_Modes;
	
	array<ref EL_SirenMode> GetModes()
	{
		return m_Modes;
	}
	
	void Insert(EL_LightComponent light)
	{
		foreach(EL_SirenMode mode : m_Modes)
		{
			mode.Insert(light);
		}
	}
	
	EL_SirenMode Find(string name)
	{
		int i = 0;
		while(i < m_Modes.Count() && m_Modes[i].GetName() != name)
		{
			i++;
		}
		if(i < m_Modes.Count()) return m_Modes[i];
		return null;
	}
}