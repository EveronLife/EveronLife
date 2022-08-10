modded class SCR_InventoryStorageManagerComponent
{
    //------------------------------------------------------------------------------------------------
    override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
    {
        super.OnItemAdded(storageOwner, item);
        
		InventoryItemComponent invItem = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		PrintFormat("Item %1 now has parent %2.", EL_Utils.GetPrefabName(item), invItem.GetParentSlot());
		
        EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(item.FindComponent(EL_PersistenceComponent));
        
        if(persistenceComponent) persistenceComponent.OnStorageParentChanged(item, storageOwner.GetOwner());
    }

    //------------------------------------------------------------------------------------------------
    override protected void OnItemRemoved(BaseInventoryStorageComponent storageOwner, IEntity item)
    {
        super.OnItemRemoved(storageOwner, item);
        
		InventoryItemComponent invItem = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		PrintFormat("Item %1 now has parent %2.", EL_Utils.GetPrefabName(item), invItem.GetParentSlot());
		
        EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(item.FindComponent(EL_PersistenceComponent));
        
        if(persistenceComponent) persistenceComponent.OnStorageParentChanged(item, null);
    }
}
