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
	static bool TryBuy(notnull IEntity player, ResourceName prefabToBuy, int price, int amount)
	{
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;

		if (EL_MoneyUtils.GetCash(inventoryManager) < price)
			return false;

		EL_InventoryUtils.AddAmount(player, prefabToBuy, amount);
		RemoveCash(player, price * amount);
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool TrySell(notnull IEntity player, ResourceName prefabToSell, int price, int amount)
	{
		if (prefabToSell.IsEmpty())
			return false;

		EL_InventoryUtils.RemoveAmount(player, prefabToSell, amount);
		AddCash(player, price * amount);
		
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
