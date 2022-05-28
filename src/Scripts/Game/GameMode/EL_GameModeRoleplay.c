class EL_GameModeRoleplayClass: SCR_BaseGameModeClass
{
};

class EL_GameModeRoleplay: SCR_BaseGameMode
{
	protected bool m_GatherHintShown = false;
	protected bool m_TraderHintShown = false;
	
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if(!System.IsConsoleApp()) //No GUI, so no events for it
		{
			Event_OnControllableSpawned.Insert(OnPlayerSpawnedClient);
		}
	}
	
	static EL_GameModeRoleplay GetInstance()
	{
		return EL_GameModeRoleplay.Cast(GetGame().GetGameMode());
	}
	
	void OnPlayerSpawnedClient(IEntity entity)
	{
		GetGame().GetCallqueue().CallLater(ShowInitalGatherHint, 5000);
	}
	
	void ShowInitalGatherHint()
	{
		if(m_GatherHintShown) return;
		
		m_GatherHintShown = true;
		
		SCR_HintManagerComponent.GetInstance().ShowCustomHint("You can gather resources from the apple trees around you. Look at their trunk and hold F.", "Gathering resources", 20.0);
	}
	
	void ShowInitalTraderHint()
	{
		//Must have already gathered to reach this code, so disable gather hint as well.
		m_GatherHintShown = true;
		
		if(m_TraderHintShown) return;
		
		m_TraderHintShown = true;
		
		SCR_HintManagerComponent.GetInstance().ShowCustomHint("Great you just gathered your first apple! Now you can go to the NPC and trade them for pistol ammo.", "Trading with NPCs", 20.0);
	}
}
