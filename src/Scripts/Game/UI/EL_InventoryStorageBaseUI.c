modded class SCR_InventoryStorageBaseUI
{
	protected override int FindItem(notnull InventoryItemComponent pInvItem)
	{
		IEntity entity = pInvItem.GetOwner();
		if (!entity)
		{
			return super.FindItem(pInvItem);
		}
			
		//! If the item has a stack component then each entity must be shown unique
		EL_InventoryStackComponent stackComponent = EL_InventoryStackComponent.Cast(entity.FindComponent(EL_InventoryStackComponent));
		if (!stackComponent)
		{
			return super.FindItem(pInvItem);
		}

		return -1;
	}
};
