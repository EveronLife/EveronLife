class EL_MoneyUtils
{
	static const ResourceName PREFAB_CASH = "{5439738849229352}Prefabs/Items/Currencies/MoneyStack.et";

	//------------------------------------------------------------------------------------------------
	static bool CanBuy(notnull IEntity player, ResourceName prefabToBuy, int price, out string errorMessage)
	{
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;

		if (EL_MoneyUtils.GetCash(inventoryManager) < price)
		{
			errorMessage = "Can't afford";
			return false;
		}
		
		if (!EL_InventoryUtils.CanSpawnPrefabToStorage(inventoryManager, prefabToBuy))
		{
			errorMessage = "Inventory full";
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool TryBuy(notnull IEntity player, ResourceName prefabToBuy, int price)
	{
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;

		if (EL_MoneyUtils.GetCash(inventoryManager) < price)
			return false;
		
		//Spawn bought Item
		bool insertSucess = inventoryManager.TrySpawnPrefabToStorage(prefabToBuy);
		if (!insertSucess)
			return false;

		RemoveCash(player, price);
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool TrySell(notnull IEntity player, ResourceName prefabToSell, int price)
	{
		if (prefabToSell.IsEmpty())
			return false;
		SCR_PrefabNamePredicate prefabNamePredicate();
		prefabNamePredicate.prefabName = prefabToSell;

		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;

		//Delete sold Item
		bool deleteSucess = inventoryManager.TryDeleteItem(inventoryManager.FindItem(prefabNamePredicate));
		if (!deleteSucess)
			return false;

		//Spawn Money
		AddCash(player, price);

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get the total amount of cash the target has across all storages
	//! \return the total cash or -1 on faulty opertations.
	static int GetCash(InventoryStorageManagerComponent target)
	{
		return EL_InventoryUtils.GetAmount(target, PREFAB_CASH);
	}

	//------------------------------------------------------------------------------------------------
	//! \ref GetCash(InventoryStorageManagerComponent)
	static int GetCash(IEntity target)
	{
		return EL_InventoryUtils.GetAmount(target, PREFAB_CASH);
	}

	//------------------------------------------------------------------------------------------------
	//! Add cash to the target
	//! \param amount Cash to add
	//! \return the amount of cash added or 0 on faulty opertations.
	static int AddCash(InventoryStorageManagerComponent target, int amount)
	{
		return EL_InventoryUtils.AddAmount(target, PREFAB_CASH, amount);
	}

	//------------------------------------------------------------------------------------------------
	//! \ref AddCash(InventoryStorageManagerComponent, int)
	static int AddCash(IEntity target, int amount)
	{
		return EL_InventoryUtils.AddAmount(target, PREFAB_CASH, amount);
	}

	//------------------------------------------------------------------------------------------------
	//! Remove cash from the target
	//! \param amount Cash to remove
	//! \return the amount of cash added or 0 on faulty opertations.
	static int RemoveCash(InventoryStorageManagerComponent target, int amount)
	{
		return EL_InventoryUtils.RemoveAmount(target, PREFAB_CASH, amount);
	}

	//------------------------------------------------------------------------------------------------
	//! \ref RemoveCash(InventoryStorageManagerComponent, int)
	static int RemoveCash(IEntity target, int amount)
	{
		return EL_InventoryUtils.RemoveAmount(target, PREFAB_CASH, amount);
	}
}
