class EL_SirenManagerComponentClass : ScriptComponentClass
{
}

/**
\brief Manages siren modes, light animations and knob procedural animation
**/
class EL_SirenManagerComponent : ScriptComponent
{
	// Sound volume when on/off
	protected const float SOUND_OFF = 0;
	protected const float SOUND_ON = 1;
	
	[Attribute()]
	protected ref EL_SirenModes m_Modes;
	
	// Used for sending only the necessary signals to m_SoundComp
	protected EL_SirenMode m_CurrentMode;
	// For JIPs in MP
	protected int m_CurrentModeIndex;
	
	protected EL_SirenKnobComponent m_Knob;
	protected SignalsManagerComponent m_KnobSigComp;
	
	// Sound component of the vehicle
	protected SoundComponent m_SoundComp;
	
	override void OnPostInit(IEntity owner)
	{
		m_SoundComp = SoundComponent.Cast(owner.FindComponent(SoundComponent));
		m_CurrentModeIndex = m_Modes.Find("default");
		GetGame().GetInputManager().AddActionListener("EL_CycleSirenModesSingle", EActionTrigger.DOWN, CycleModesSingle);
		GetGame().GetInputManager().AddActionListener("EL_CycleSirenModesDouble", EActionTrigger.DOWN, CycleModesDouble);
	}
	
	//------------------------------------------------------------------------------------------------
	void CycleModesSingle()
	{
		SetModeStr(m_CurrentMode.GetSingleClickMode());
	}
	
	//------------------------------------------------------------------------------------------------
	void CycleModesDouble()
	{
		SetModeStr(m_CurrentMode.GetDoubleClickMode());
	}
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief Called when a light component registers itself
	\param light - the light that is registering itself
	**/
	void RegisterLight(EL_SirenLightComponent light)
	{
		if(m_Modes) m_Modes.InsertLight(light);
	}
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief Called when a knob component registers itself
	\param knob - the knob that is registering itself
	**/
	void RegisterKnob(EL_SirenKnobComponent knob)
	{
		if(m_Knob) Print("Siren with multiple knobs", LogLevel.WARNING);
		else 
		{
			m_Knob = knob;
			m_KnobSigComp = SignalsManagerComponent.Cast(knob.GetOwner().FindComponent(SignalsManagerComponent));
			SetMode();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief Sets the mode of the siren/horn. Called from SetMode()
	\param mode - the mode that is going to be set
	**/
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
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief Sets the signals of the knob procedural animation. Called from SetMode()
	\param mode - the mode that is going to be set
	**/
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
		m_SoundComp.SoundEventBone("SOUND_VEHICLE_CLOSE_LIGHT_ON", "Scene_Root");
	}
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief Sets the current mode based on m_CurrentModeIndex. Changes knob position and siren sounds
	**/
	protected void SetMode()
	{
		EL_SirenMode mode = m_Modes.GetMode(m_CurrentModeIndex);
		m_SoundComp.SoundEvent("Vehicle_Horn");
		if(mode)
		{
			if(m_Knob) SetKnobMode(mode);
			
			if(m_SoundComp) SetSirenMode(mode);
			if(mode.GetName() != "default") 
			{
				m_SoundComp.SetSignalValueStr("animSelector", 1);
				m_SoundComp.SoundEventBone("SOUND_VEHICLE_HORN", "Scene_Root");
				
				m_SoundComp.SetSignalValueStr("animSelector", 0);
			}
		}
		
		m_CurrentMode = mode;
	}
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief Sets mode by name. See SetMode()
	\param name - name of the mode
	**/
	void SetModeStr(string name)
	{
		if(!m_Modes) return;
		int nextModeIndex = m_Modes.Find(name);
		if(nextModeIndex == m_CurrentModeIndex) return;
		m_CurrentModeIndex = nextModeIndex;
		SetMode();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool RplSave(ScriptBitWriter writer)
	{
		writer.WriteIntRange(m_CurrentModeIndex, 0, EL_SirenMode.N_MODES);
		writer.WriteFloat(m_CurrentMode.GetAnimation().GetTimeInLoop());
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool RplLoad(ScriptBitReader reader)
	{
		float timeSlice;
		reader.ReadIntRange(m_CurrentModeIndex, 0, EL_SirenMode.N_MODES);
		SetMode();
		reader.ReadFloat(timeSlice);
		m_CurrentMode.GetAnimation().Tick(timeSlice + Replication.Time());
		return true;
	}
}


/**
Contains all the information about the siren mode. Animation, sounds and knob procedural animation signals
**/
[BaseContainerProps(), SCR_BaseContainerCustomTitleField("m_Name", "%1")]
class EL_SirenMode
{
	// Available modes to the user
	static const ref ParamEnumArray MODE_NAMES =
	{
		new ParamEnum("default", "0"),
		new ParamEnum("Mode1", "1"),
		new ParamEnum("Mode2", "2"),
		new ParamEnum("Mode3", "3"),
		new ParamEnum("Mode4", "4"),
		new ParamEnum("Mode5", "5"),
		new ParamEnum("Mode6", "6"),
		new ParamEnum("Mode7", "7")
	};
	
	// Number of modes available, used to reduce network traffic. Increase in powers of 2
	static const int N_MODES = 8;
	// Available sounds
	static protected const ref ParamEnumArray SIREN_SOUNDS = 
	{
		new ParamEnum("Silent", "0"),
		new ParamEnum("DefaultHorn", "1"),
		new ParamEnum("SlowSiren", "2"),
		new ParamEnum("FastSiren", "3"),
		new ParamEnum("FastestSiren", "4"),
		new ParamEnum("WarningSiren", "5")
	};
	
	[Attribute("default", uiwidget: UIWidgets.ComboBox, enums: MODE_NAMES)]
	protected string m_Name;
	
	[Attribute("default", uiwidget: UIWidgets.ComboBox, enums: MODE_NAMES)]
	protected string m_NextModeSingleClick;
	
	[Attribute("default", uiwidget: UIWidgets.ComboBox, enums: MODE_NAMES)]
	protected string m_NextModeDoubleClick;
	
	[Attribute()]
	protected ref EL_LightAnimation m_Animation;
	
	[Attribute(desc: "Sound that should be played when the horn is pressed.", uiwidget: UIWidgets.ComboBox, enums: SIREN_SOUNDS)]
	protected string m_SoundWhenPressed;
	
	[Attribute(desc: "Sound that should be played when horn is released", uiwidget: UIWidgets.ComboBox, enums: SIREN_SOUNDS)]
	protected string m_SoundWhenReleased;
	
	[Attribute(desc: "Controls the knob position for each mode. Each pair contains the name of the procedural animation signal and it's value.")]
	protected ref array<ref SignalValuePair> m_KnobSignals;
	
	//------------------------------------------------------------------------------------------------
	string GetName()
	{
		return m_Name;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetDoubleClickMode()
	{
		return m_NextModeDoubleClick;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetSingleClickMode()
	{
		return m_NextModeSingleClick;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetSirenActive()
	{
		return m_SoundWhenPressed;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetSirenInactive()
	{
		return m_SoundWhenReleased;
	}
	
	//------------------------------------------------------------------------------------------------
	EL_LightAnimation GetAnimation()
	{
	 	return m_Animation;
	}
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief Inserts a light into the animation. Called when a light registers itself
	\param light - light to be inserted
	**/
	void InsertLight(EL_SirenLightComponent light)
	{
		if(m_Animation) m_Animation.InsertLight(light);
	}
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief Get the signal value pairs of the knob procedural animation attached to this mode
	\return signal value pairs
	**/
	array<ref SignalValuePair> GetKnobSignals()
	{
		return m_KnobSignals;
	}
}


/**
Holds name and value of a procedural animation signal. 
**/
[BaseContainerProps()]
class SignalValuePair
{
	[Attribute(desc: "Name of the signal to send to the procedural animation.")]
	protected string m_Name;
	
	[Attribute(desc: "Value of the procedural animation signal specified above.")]
	protected float m_Value;
	
	//------------------------------------------------------------------------------------------------
	string GetName()
	{
		return m_Name;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetValue()
	{
		return m_Value;
	}
}


/**
Holds all the modes tht the user set. Primary function is to enable saving the modes into a single config prefab
**/
[BaseContainerProps()]
class EL_SirenModes
{
	[Attribute()]
	protected ref array<ref EL_SirenMode> m_Modes;
	
	//------------------------------------------------------------------------------------------------
	array<ref EL_SirenMode> GetModes()
	{
		return m_Modes;
	}
	
	//------------------------------------------------------------------------------------------------
	/**
 	\brief Insert the light into each mode. Called when a light registers itself
	\param light - the light to be inserted
	**/
	void InsertLight(EL_SirenLightComponent light)
	{
		foreach(EL_SirenMode mode : m_Modes)
		{
			mode.InsertLight(light);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/**
 	\brief Gets a mode by index
	\param index - index of the mode
	\return the mode with specified index, null if index is invalid
	**/
	EL_SirenMode GetMode(int index)
	{
		if(index < 0 || index > m_Modes.Count() - 1) return null;
		return m_Modes[index];
	}
	
	//------------------------------------------------------------------------------------------------
	/**
 	\brief Finds a mode by name
	\param name - name of the mode being searched 
	\return the index of the found mode or -1 if not found
	**/
	int Find(string name)
	{
		int i = 0;
		while(i < m_Modes.Count() && m_Modes[i].GetName() != name)
		{
			i++;
		}
		if(i < m_Modes.Count()) return i;
		return -1;
	}
}
