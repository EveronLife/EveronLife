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
}
