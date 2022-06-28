class EL_RestrictedInventoryStorageComponentClass : SCR_UniversalInventoryStorageComponentClass
{
}

class EL_RestrictedInventoryStorageComponent : SCR_UniversalInventoryStorageComponent
{
    [Attribute("", UIWidgets.ResourceAssignArray, "Choose prefabs", "et", NULL, "Allowed Items Configuration")]
    protected ref array<ResourceName> m_aTradablePrefabs;
    
	//------------------------------------------------------------------------------------------------
    override bool CanStoreItem(IEntity item, int slotID)
    {   
        return m_aTradablePrefabs.Contains(item.GetPrefabData().GetPrefabName());
    }
}
