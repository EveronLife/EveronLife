class EL_OpenTraderAction : SCR_InventoryAction
{
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		manager.SetStorageToOpen(pOwnerEntity);
		manager.OpenInventory();
	}
}
