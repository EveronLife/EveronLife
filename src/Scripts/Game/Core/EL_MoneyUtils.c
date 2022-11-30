class EL_MoneyUtils
{
	static const ResourceName MONEY_PREFAB = "{FDEE11D818A4C675}Prefabs/Items/Money/DollarBill.et";

	static ref SCR_PrefabNamePredicate m_pPrefabNamePredicate = new SCR_PrefabNamePredicate();

	//------------------------------------------------------------------------------------------------
	static bool CanBuy(notnull IEntity player, IEntity entToBuy, int price, out string errorMessage)
	{
		m_pPrefabNamePredicate.prefabName = MONEY_PREFAB;

		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;

		//Find Money
		array<IEntity> moneyItems = new array<IEntity>();
		inventoryManager.FindItems(moneyItems, m_pPrefabNamePredicate);

		if (moneyItems.Count() < price)
		{
			errorMessage = "Can't afford";
			return false;
		}
		
		if (!inventoryManager.CanInsertItem(entToBuy))
		{
			errorMessage = "Inventory full";
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool TryBuy(notnull IEntity player, ResourceName prefabToBuy, int price)
	{
		m_pPrefabNamePredicate.prefabName = MONEY_PREFAB;

		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;

		//Find Money
		array<IEntity> moneyItems = new array<IEntity>();
		inventoryManager.FindItems(moneyItems, m_pPrefabNamePredicate);

		if (moneyItems.Count() < price)
			return false;
		//Delete Money
		for (int i=0; i < price; i++)
		{
			inventoryManager.TryDeleteItem(moneyItems[i]);
		}

		//Spawn bought Item
		bool insertSucess = inventoryManager.TrySpawnPrefabToStorage(prefabToBuy);
		if (!insertSucess)
			return false;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool TrySell(notnull IEntity player, ResourceName prefabToSell, int price)
	{
		if (prefabToSell.IsEmpty())
			return false;
		m_pPrefabNamePredicate.prefabName = prefabToSell;

		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;

		//Delete sold Item
		bool deleteSucess = inventoryManager.TryDeleteItem(inventoryManager.FindItem(m_pPrefabNamePredicate));
		if (!deleteSucess)
			return false;

		//Spawn Money
		for (int i=0; i < price; i++)
		{
			inventoryManager.TrySpawnPrefabToStorage(MONEY_PREFAB);
		}

		return true;
	}
}
