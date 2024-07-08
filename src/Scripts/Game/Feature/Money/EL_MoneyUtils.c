class EL_MoneyUtils
{
	static const ResourceName PREFAB_CASH = "{5439738849229352}Prefabs/Items/Currencies/MoneyStack.et";

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
	//! \return the amount of cash removed or 0 on faulty opertations.
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
