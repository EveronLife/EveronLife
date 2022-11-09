class EL_InventoryUtils
{
	//------------------------------------------------------------------------------------------------
	static bool IsStorageHierachyRoot(notnull IEntity item)
	{
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		return !itemComponent.GetParentSlot();
	}

	//------------------------------------------------------------------------------------------------
	static IEntity GetStorageHierachyRoot(notnull IEntity item)
	{
		while (item)
		{
			InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (!itemComponent || !itemComponent.GetParentSlot()) break;
			item = itemComponent.GetParentSlot().GetStorage().GetOwner();
		}
		return item;
	}

	//------------------------------------------------------------------------------------------------
	static InventoryStorageManagerComponent GetResponsibleStorageManager(notnull IEntity item)
	{
		while (item)
		{
			InventoryStorageManagerComponent manager = EL_ComponentFinder<InventoryStorageManagerComponent>.Find(item);
			if (manager) return manager;
			InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (!itemComponent || !itemComponent.GetParentSlot()) break;
			item = itemComponent.GetParentSlot().GetStorage().GetOwner();
		}
		return null;
	}

	//! Returns the Owner of the storage the item is in
	//! \param item Item Entity to get Owner from
	//! \return Entity Storage Owner
	static IEntity GetStorageOwner(notnull IEntity item)
	{
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		InventoryStorageSlot slot = itemComponent.GetParentSlot();
		if (!slot) return null;

		BaseInventoryStorageComponent itemParentStorage = slot.GetStorage();
		if (!itemParentStorage) return null;

		InventoryStorageSlot parentSlot = itemParentStorage.GetParentSlot();
		if (!parentSlot) return null;

		return parentSlot.GetStorage().GetOwner();
	}

	//------------------------------------------------------------------------------------------------
	static bool DropItem(notnull IEntity character, notnull IEntity item)
	{
		InventoryItemComponent itemComponent = EL_ComponentFinder<InventoryItemComponent>.Find(item);
		if (!itemComponent) return false;

		InventoryStorageSlot parentSlot = itemComponent.GetParentSlot();
		if (!parentSlot) return false;

		InventoryStorageManagerComponent inventoryManager = EL_ComponentFinder<InventoryStorageManagerComponent>.Find(character);
		BaseInventoryStorageComponent storage = parentSlot.GetStorage();
		if (!storage || storage.GetOwner() != character) return false;
		return inventoryManager.TryRemoveItemFromStorage(item, storage);
	}
}
