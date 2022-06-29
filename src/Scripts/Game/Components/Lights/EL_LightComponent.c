class EL_LightComponentClass : ScriptComponentClass
{
	
}



class EL_LightComponent : ScriptComponent 
{
	
	[Attribute()]
	protected string m_Name;
	
	
	[Attribute()]
	protected ref array<ref EL_LightChild> m_Lights;
	
	
	protected bool m_isOn = false;
	
	const int EMISSIVE_ON = 1;
	const int EMISSIVE_OFF = 0;
	
	protected ParametricMaterialInstanceComponent m_Material;
	protected AnimationPlayerComponent m_Anim;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		m_Material = ParametricMaterialInstanceComponent.Cast(owner.FindComponent(ParametricMaterialInstanceComponent));
		m_Anim = AnimationPlayerComponent.Cast(owner.FindComponent(AnimationPlayerComponent));
		Register(owner);
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
			Play();
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
			Stop();
			m_isOn = false;
		}
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
	void Register(IEntity owner)
	{
		IEntity parent = owner.GetParent();
		EL_SirenManagerComponent manager;
		
			Print(parent);
		while(parent && !manager)
		{
			manager = EL_SirenManagerComponent.Cast(parent.FindComponent(EL_SirenManagerComponent));
			parent = parent.GetParent();
		}
		if(manager) manager.Register(this);
		else Print("Light component with no manager", LogLevel.WARNING);
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
	
	string GetName()
	{
		return m_Name;
	}
	void ~EL_LightComponent()
	{
		DestroyLights();
	}
	
}