[BaseContainerProps()]
class EL_BaseEntry : ScriptAndConfig
{
	protected ref array<EL_LightComponent> m_AffectedLights = {};
	
	sealed void OnRegister(EL_LightComponent light)
	{
		if(Affects(light)) m_AffectedLights.Insert(light);
	}
	
	void OnExecute(EL_LightAnimation animation)
	{
		foreach(EL_LightComponent light : m_AffectedLights)
		{
			Act(light);
		}
	}
	
	protected bool Affects(EL_LightComponent light)
	{
		return false;
	}
	
	protected void Act(EL_LightComponent light);
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
		if(m_LoopsRemaining)
		{
			m_LoopsRemaining--;
			animation.GoBack(m_Steps);
		}
		else
		{
			m_LoopsRemaining = m_LoopCount;
		}
	}
}

[BaseContainerProps(), SCR_BaseContainerStaticTitleField("All on")]
class EL_TurnOnEntry : EL_BaseEntry
{
	[Attribute()]
	protected string m_Name;
	
	override bool Affects(EL_LightComponent light)
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
	
	override void Act(EL_LightComponent light)
	{
		light.TurnOn();
	}
}

[SCR_BaseContainerStaticTitleField("All off")]
class EL_TurnOffEntry : EL_BaseEntry
{
	[Attribute()]
	protected string m_Name;
	
	override bool Affects(EL_LightComponent light)
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
	
	override void Act(EL_LightComponent light)
	{
		light.TurnOff();
	}
}