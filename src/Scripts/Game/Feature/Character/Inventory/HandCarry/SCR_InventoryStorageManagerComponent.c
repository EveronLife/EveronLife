modded class SCR_InventoryStorageManagerComponent
{
	//------------------------------------------------------------------------------------------------
	override void InsertItem(IEntity pItem, BaseInventoryStorageComponent pStorageTo = null, BaseInventoryStorageComponent pStorageFrom = null, SCR_InvCallBack cb = null)
	{
		// Stop pickup animations on hand carry items
		if (m_CharacterController && EL_Component<EL_HandCarryComponent>.Find(pItem))
		{
			m_CharacterController.TryPlayItemGesture(EItemGesture.EItemGestureNone);
		}

		super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemAdded(storageOwner, item);
		EL_RefreshHandCarryMenu(item);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemRemoved(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemRemoved(storageOwner, item);
		EL_RefreshHandCarryMenu(item);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_RefreshHandCarryMenu(IEntity item)
	{
		if (!EL_Component<EL_HandCarryComponent>.Find(item)) return;

		SCR_InventoryMenuUI inventoryMenu = SCR_InventoryMenuUI.EL_GetCurrentInstance();
		if (!inventoryMenu) return;

		SCR_InventoryStoragesListUI storageList = SCR_InventoryStoragesListUI.Cast(inventoryMenu.GetStorageList());
		if (storageList) storageList.EL_UpdateHandSlot();
	}
}
