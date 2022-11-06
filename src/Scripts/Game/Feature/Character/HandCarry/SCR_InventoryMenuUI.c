modded class SCR_InventoryStoragesListUI
{
	//------------------------------------------------------------------------------------------------
	void SCR_InventoryStoragesListUI(
		BaseInventoryStorageComponent storage,
		ESlotID slotID = ESlotID.SLOT_ANY,
		SCR_InventoryMenuUI menuManager = null,
		int iPage = 0,
		array<BaseInventoryStorageComponent> aTraverseStorage = null)
	{
		EL_HandInventoryStorageComponent handStorage = EL_ComponentFinder<EL_HandInventoryStorageComponent>.Find(storage.GetOwner());
		if (handStorage)
		{ 
			array<IEntity> outItems();
			handStorage.GetAll(outItems);
			if (outItems.IsEmpty()) return;
		}

		// Insert a row for the in hand item
		m_iMaxRows++;
		m_iMatrix.InsertRow(SCR_MatrixRow(m_iMaxColumns));
	}
}
