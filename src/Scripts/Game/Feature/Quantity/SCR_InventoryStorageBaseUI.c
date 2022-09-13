modded class SCR_InventoryStorageBaseUI
{
	//------------------------------------------------------------------------------------------------
	override protected int FindItem(notnull InventoryItemComponent pInvItem)
	{
		// If the item has a quantity component then each entity must be shown unique
		if (pInvItem.GetOwner().FindComponent(EL_QuantityComponent)) return -1;

		return super.FindItem(pInvItem);
	}
}
