class EL_GlobalInventoryStorageManagerClass : GenericEntityClass
{
}

class EL_GlobalInventoryStorageManager : GenericEntity
{
}

class EL_GlobalInventoryStorageManagerComponentClass : InventoryStorageManagerComponentClass
{
}

class EL_GlobalInventoryStorageManagerComponent : InventoryStorageManagerComponent
{
	protected static const ResourceName PREFAB = "{B5D2C533815D511D}Prefabs/Core/EL_GlobalInventoryStorageManager.et";
	protected static EL_GlobalInventoryStorageManagerComponent s_pInstance;
	
	static EL_GlobalInventoryStorageManagerComponent GetInstance()
	{
		if(!s_pInstance && Replication.IsServer())
		{
			EL_GlobalInventoryStorageManager storageManager = EL_GlobalInventoryStorageManager.Cast(EL_Utils.SpawnEntityPrefab(PREFAB, "0 100 0"));
			if(!storageManager) return null;
			
			s_pInstance = EL_GlobalInventoryStorageManagerComponent.Cast(storageManager.FindComponent(EL_GlobalInventoryStorageManagerComponent));
			if(!s_pInstance)
			{
				SCR_EntityHelper.DeleteEntityAndChildren(storageManager);
			}
		}
		
		return s_pInstance;
	}
}
