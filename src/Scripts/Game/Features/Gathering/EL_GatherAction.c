class EL_GatherAction : ScriptedUserAction
{
	[Attribute("", UIWidgets.EditBox, desc: "Display name of what is being gathered")]
	private string m_GatherItemDisplayName;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab what item is gathered")]
	private ResourceName m_GatherItemPrefab;
		
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Play sound
		auto replication = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		
		//Todo: Replace SCR_InventoryStorageManagerComponent RPL setup with own function or better synced sound alternative
		auto inventoryManager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		inventoryManager.PlayItemSound(replication.Id(), "SOUND_PICK_UP");
		
		//Spawn item
		inventoryManager.TrySpawnPrefabToStorage(m_GatherItemPrefab);
		
		//Show hint what to do with the gathered item
		EL_GameModeRoleplay.GetInstance().ShowInitalTraderHint();
	}
	
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Gather %1", m_GatherItemDisplayName);
		return true;
	}
	
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
 	}
}
