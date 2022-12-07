modded class SCR_InventoryStorageBaseUI
{
	//------------------------------------------------------------------------------------------------
	override protected int FindItem(notnull InventoryItemComponent pInvItem)
	{
		// If the item has a quantity component then each entity must be shown unique
		if (pInvItem.GetOwner().FindComponent(EL_QuantityComponent)) return -1;

		return super.FindItem(pInvItem);
	}

	//------------------------------------------------------------------------------------------------
	override protected void UpdateOwnedSlots(notnull array<IEntity> pItemsInStorage)
	{
		array<ResourceName> prefabAggregationKeys();
		array<ref array<IEntity>> prefabAggregationValues();

		foreach (IEntity item : pItemsInStorage)
		{
			string key = EL_Utils.GetPrefabName(item);
			int idx = prefabAggregationKeys.Find(key);
			
			if (idx != -1)
			{
				 prefabAggregationValues.Get(idx).Insert(item);
			}
			else
			{
				prefabAggregationKeys.Insert(key);
				prefabAggregationValues.Insert({item});
			}
		}

		pItemsInStorage.Clear();

		foreach (array<IEntity> items : prefabAggregationValues)
		{
			foreach (IEntity item : items)
			{
				pItemsInStorage.Insert(item);
			}
		}

		super.UpdateOwnedSlots(pItemsInStorage);
	}
}
