modded class SCR_InventoryStorageManagerComponent
{
	//------------------------------------------------------------------------------------------------
	override void InsertItem(IEntity pItem, BaseInventoryStorageComponent pStorageTo = null, BaseInventoryStorageComponent pStorageFrom = null, SCR_InvCallBack cb = null)
	{
		// Stop pickup animations on hand carry items
		if (m_pCharacterController && EL_ComponentFinder<EL_HandCarryComponent>.Find(pItem))
		{
			m_pCharacterController.TryPlayItemGesture(EItemGesture.EItemGestureNone);
		}

		super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
	}
}
