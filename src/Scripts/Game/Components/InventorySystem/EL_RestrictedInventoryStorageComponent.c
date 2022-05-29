class EL_RestrictedInventoryStorageComponentClass :SCR_UniversalInventoryStorageComponentClass
{
}

class EL_RestrictedInventoryStorageComponent : SCR_UniversalInventoryStorageComponent
{
    [Attribute("", UIWidgets.ResourceAssignArray, "Choose prefabs", "et", NULL, "Allowed Items Configuration")]
    protected ref array<ResourceName> m_TradablePrefabs;
    
    override bool CanStoreItem(IEntity item, int slotID)
    {   
		Print("testtesttest");   
        return m_TradablePrefabs.Contains(item.GetPrefabData().GetPrefabName());
    }
}
