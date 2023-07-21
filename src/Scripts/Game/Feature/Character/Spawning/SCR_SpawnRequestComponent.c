modded class SCR_SpawnRequestComponent
{
	//------------------------------------------------------------------------------------------------
	protected override void OnPostInit(IEntity owner)
	{
		// Mute warnings on current minimalistic EPF respawn system "hotfix"
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		if (respawnSystem && respawnSystem.IsInherited(EL_RespawnSystemComponent))
			return;
		
		super.OnPostInit(owner);
	}
}
