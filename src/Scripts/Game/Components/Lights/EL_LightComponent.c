class EL_LightComponentClass : ScriptComponentClass
{
	
}



class EL_LightComponent : ScriptComponent 
{
	
	[Attribute(desc: "EL_SirenManagerComponent uses this name to animate the light. Can be left blank if all light are changed at once")]
	protected string m_Name;
	
	[Attribute()]
	protected ref array<ref EL_LightChild> m_Lights;
	
	[Attribute()]
	protected bool m_OverrideEmissiveColor;
	
	[Attribute("1 1 1", uiwidget: UIWidgets.ColorPicker)]
	protected vector m_EmissiveColor;
	
	[Attribute("2", uiwidget: UIWidgets.Slider, params: "0 100")]
	protected int m_EmissiveMultiplier;
	
	protected bool m_isOn = true;
	
	// Emissive multiplier when on/off
	const int EMISSIVE_OFF = 0;
	
	
	protected ParametricMaterialInstanceComponent m_Material;
	protected AnimationPlayerComponent m_Anim;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		m_Material = ParametricMaterialInstanceComponent.Cast(owner.FindComponent(ParametricMaterialInstanceComponent));
		m_Anim = AnimationPlayerComponent.Cast(owner.FindComponent(AnimationPlayerComponent));
		Register();
		if(!m_Material)
		{
			Print("EL_LightComponent requires ParametricMaterialInstanceComponent", LogLevel.ERROR);
			return;
		}
		if(m_OverrideEmissiveColor) m_Material.SetEmissiveColor(Color.FromVector(m_EmissiveColor.Normalized()).PackToInt());
		m_Material.SetEmissiveMultiplier(m_EmissiveMultiplier);
		
		
	}
	
	//------------------------------------------------------------------------------------------------
	// \return true if the emissive has LightEntities attached to it
	bool HasLights()
	{
		if(m_Lights.Count()) return true;
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateLightEntitiesLV()
	{
		foreach(EL_LightChild light : m_Lights)
		{
			light.UpdateLV();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SpawnLights()
	{
		foreach(EL_LightChild light :  m_Lights)
		{
			light.Spawn(GetOwner());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void TurnOn()
	{
		if(!m_isOn)
		{
			if(m_Material) m_Material.SetEmissiveMultiplier(m_EmissiveMultiplier);
			SpawnLights();
			m_isOn = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void TurnOff()
	{
		if(m_isOn)
		{
			if(m_Material)
				m_Material.SetEmissiveMultiplier(EMISSIVE_OFF);
			DestroyLights();
			m_isOn = false;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Toggle()
	{
		if(IsOn()) TurnOff();
		else TurnOn();
	}
	
	//------------------------------------------------------------------------------------------------
	void ToggleAnim()
	{
		if(IsPlaying()) Stop();
		else Play();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsOn()
	{
		return m_isOn;
	}
	
	//------------------------------------------------------------------------------------------------
	void Play()
	{
		if(m_Anim && !m_Anim.IsPlaying())
			m_Anim.Play(GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	void Stop()
	{
		if(m_Anim && m_Anim.IsPlaying())
			m_Anim.Stop(GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsPlaying()
	{
		if(m_Anim)
			return m_Anim.IsPlaying();
		return false;
	}

	//------------------------------------------------------------------------------------------------
	void DestroyLights()
	{
		foreach(EL_LightChild light :  m_Lights)
		{
			light.Destroy();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Register()
	{
		IEntity parent = GetOwner().GetParent();
		EL_SirenManagerComponent manager;
		
		while(parent && !manager)
		{
			manager = EL_SirenManagerComponent.Cast(parent.FindComponent(EL_SirenManagerComponent));
			parent = parent.GetParent();
		}
		if(manager) manager.RegisterLight(this);
		else if (GetGame()) Print("Light component with no manager", LogLevel.WARNING);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetName()
	{
		return m_Name;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool RplSave(ScriptBitWriter writer)
	{
		writer.WriteBool(m_isOn);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool RplLoad(ScriptBitReader reader)
	{
		reader.ReadBool(m_isOn);
		if(m_isOn) SpawnLights();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~EL_LightComponent()
	{
		DestroyLights();
	}
	
}

[BaseContainerProps()]
class EL_LightChild
{
	[Attribute(defvalue: "0", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(LightType), desc: "Point is light that shines to all directions. Spot shines to a specific direction.")]
	private LightType m_eLightType;	
	
	[Attribute("0 0 0", UIWidgets.EditBox, "Light offset in local space from the pivot point")]
	private vector m_vLightOffset;
	
	[Attribute("1 1 1", UIWidgets.ColorPicker, "Color of the light")]
	private vector m_vLightColor;
	
	[Attribute("0 0 0", UIWidgets.EditBox, "Direction of the light cone (useful for LightType.SPOT only).")]
	private vector m_vLightConeDirection;
	
	[Attribute("130", UIWidgets.EditBox, "Angle of the light cone (useful for LightType.SPOT only).")]
	private float m_fConeAngle;
	
	[Attribute("30", UIWidgets.EditBox, "Radius of the light effect")]
	private float m_fRadius;
	
	[Attribute(desc: "Parent bone")]
	protected string m_PivotPoint;
	
	[Attribute("0.01", desc: "Near plane of the light", params: "0.01 2")]
	protected float m_NearPlane;
	
	[Attribute("0.5", desc: "Higher value means less brightness at night", params: "0 3")]
	protected float m_NightLVAttenuation;
	
	[Attribute("15", desc: "Light intensity during night time", params: "0 20")]
	protected float m_DayLV;
	
	protected LightEntity m_LightEntity;
	
	protected ref Resource m_LightResource;
	
	protected float m_currentLV;
	
	protected float m_Sunrise, m_Sunset, m_MidnightOffset, m_NightDuration;
	
	//------------------------------------------------------------------------------------------------
	// \brief Updates the LV of the light according to camera HDR brightness
	void UpdateLV()
	{	
		if(!GetGame()) return;

		m_currentLV = m_DayLV / (1 + Math.Pow(GetGame().GetWorld().GetCameraHDRBrightness(0), 0.5) * m_NightLVAttenuation);
		
		vector color = m_vLightColor;
		color.Normalize();
		if(m_LightEntity) m_LightEntity.SetColor(Color.FromVector(color), m_currentLV);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Loads the resource into memory, spawns the entity and sets it as a child of parent
	//! \param parent The parent of the spwaned light
	void Spawn(IEntity parent)
	{
		// Setting flags manually is needed until flags input by the user gets fixed
		// Otherwise the flags reset when you save the prefab 
		LightFlags flags = LightFlags.CHEAP | LightFlags.CASTSHADOW | LightFlags.DIFFUSE_ONLY | LightFlags.DYNAMIC | LightFlags.LINEAR_FALLOFF;
		vector color = m_vLightColor;
		color.Normalize();
		m_LightEntity = LightEntity.CreateLight(m_eLightType, 
												flags, 
												m_fRadius, 
												Color.FromVector(color), 
												m_currentLV,
												m_vLightOffset,
												m_vLightConeDirection);
		m_LightEntity.SetConeAngle(m_fConeAngle);
		m_LightEntity.SetNearPlane(m_NearPlane);
		m_LightEntity.SetLensFlareType(LightLensFlareType.Disabled);
		parent.AddChild(m_LightEntity, parent.GetBoneIndex(m_PivotPoint));
	}
	
	//------------------------------------------------------------------------------------------------
	void Destroy()
	{
		delete m_LightEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~EL_LightChild()
	{
		Destroy();
	}
}