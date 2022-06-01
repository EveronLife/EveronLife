class EL_GatherAction : ScriptedUserAction
{
	[Attribute("", UIWidgets.EditBox, desc: "Display name of what is being gathered")]
	private string m_GatherItemDisplayName;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab what item is gathered")]
	private ResourceName m_GatherItemPrefab;
	
	[Attribute("false", UIWidgets.ResourceNamePicker, desc: "Delete the object after gathering?")]
	private bool m_DeleteAfterUse;
	
	[Attribute("true", UIWidgets.ResourceNamePicker, desc: "Gather Action always active?")]
	private bool m_AlwaysActive;
	
	
	//------------------------------------------------------------------------------------------------
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
		
		//Delete entity after use
		if (m_DeleteAfterUse)
			delete(pOwnerEntity);
			
	}		
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Gather %1", m_GatherItemDisplayName);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		//Upcast to BaseCrop to get grow status from any crop type
		EL_BaseCrop baseCrop = EL_BaseCrop.Cast(GetOwner());
		if (baseCrop && baseCrop.m_IsGrown)
			return true;
		
		return m_AlwaysActive;
 	}
	
	//------------------------------------------------------------------------------------------------
	//! Hide action until CanBePerformed to not show "Action [Unavailable]"
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}
}
