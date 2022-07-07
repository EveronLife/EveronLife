class EL_LightComponentClass : ScriptComponentClass
{
	
}



class EL_LightComponent : ScriptComponent 
{
	
	[Attribute(desc: "EL_SirenManagerComponent uses this name to animate the light. Can be left blank if all light are changed at once")]
	protected string m_Name;
	
	[Attribute()]
	protected ref array<ref EL_LightChild> m_Lights;
	
	
	protected bool m_isOn = true;
	
	// Emissive multiplier when on/off
	const int EMISSIVE_ON = 1;
	const int EMISSIVE_OFF = 0;
	
	protected ParametricMaterialInstanceComponent m_Material;
	protected AnimationPlayerComponent m_Anim;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		m_Material = ParametricMaterialInstanceComponent.Cast(owner.FindComponent(ParametricMaterialInstanceComponent));
		m_Anim = AnimationPlayerComponent.Cast(owner.FindComponent(AnimationPlayerComponent));
		Register();
	}
	
	//------------------------------------------------------------------------------------------------
	void TurnOn()
	{
		if(!m_isOn)
		{
			if(m_Material)
				m_Material.SetEmissiveMultiplier(EMISSIVE_ON);
			foreach(EL_LightChild light :  m_Lights)
			{
				light.Spawn(GetOwner());
			}
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
		else Print("Light component with no manager", LogLevel.WARNING);
	}
	
	//------------------------------------------------------------------------------------------------
	string GetName()
	{
		return m_Name;
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
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, params: "et", desc: "Prefab of the LightEntity that will be used. Can be left blank")]
	protected ResourceName m_LightPrefab;
	
	// Used to circumvent a game bug where cone angle set on the prefab is ignored
	[Attribute(desc: "Cone angle of the spot light. Can be ignored for point lights", params: "0 360")]
	protected float m_ConeAngle;
	
	[Attribute("PivotPoint", desc: "Position of the light relative to parent root or bone")]
	protected ref PivotPoint m_PivotPoint;
	
	protected LightEntity m_LightEntity;
	
	protected ref Resource m_LightResource;
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetPrefab()
	{
		return m_LightPrefab;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Loads the resource into memory, spawns the entity and sets it as a child of parent
	//! \param parent The parent of the spwaned light
	//! \return If it could load the resource
	bool Spawn(IEntity parent)
	{
		m_LightEntity = LightEntity.Cast(EL_Utils.SpawnAsChild(m_LightPrefab, parent, m_PivotPoint));
		if(!m_LightEntity) return false;
		if(m_ConeAngle)
			m_LightEntity.SetConeAngle(m_ConeAngle);
		m_LightEntity.SetLightFlags(LightFlags.CHEAP | LightFlags.CASTSHADOW | LightFlags.DYNAMIC | LightFlags.DIFFUSE_ONLY);
		return true;
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