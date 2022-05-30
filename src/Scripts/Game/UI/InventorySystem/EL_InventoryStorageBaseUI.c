modded class SCR_InventoryStorageBaseUI
{
	protected override int FindItem(notnull InventoryItemComponent pInvItem)
	{
		IEntity entity = pInvItem.GetOwner();
		if (!entity)
		{
			return super.FindItem(pInvItem);
		}
			
		//! If the item has a quantity component then each entity must be shown unique
		EL_InventoryQuantityComponent quantityComponent = EL_InventoryQuantityComponent.Cast(entity.FindComponent(EL_InventoryQuantityComponent));
		if (!quantityComponent)
		{
			return super.FindItem(pInvItem);
		}

		return -1;
	}
};
