modded class SCR_InventoryStoragesListUI
{
	protected const int EL_HAND_GRID_ROW = 10;

	protected SCR_InventorySlotUI m_pELHandSlot;
	protected Widget m_pELHandSlotBackground;

	//------------------------------------------------------------------------------------------------
	void EL_UpdateHandSlot()
	{
		EL_HandInventoryStorageComponent handStorage = EL_Component<EL_HandInventoryStorageComponent>.Find(m_Storage.GetOwner());
		if (!handStorage) return;

		IEntity item;
		array<IEntity> outItems();
		handStorage.GetAll(outItems);
		if (!outItems.IsEmpty()) item = outItems.Get(0);

		if (item)
		{
			InventoryItemComponent invItem = EL_Component<InventoryItemComponent>.Find(item);

			if (!m_pELHandSlot)
			{
				m_pELHandSlotBackground = CreateEmptySlotUI(EL_HAND_GRID_ROW, 0);
				m_pELHandSlot = CreateStorageSlotUI(invItem);
				Widget w = m_pELHandSlot.GetWidget();
				GridSlot.SetRow(w, EL_HAND_GRID_ROW);
				GridSlot.SetRowSpan(w, 1);
				GridSlot.SetColumn(w, 0);
				GridSlot.SetColumnSpan(w, 1);
			}
			else
			{
				m_pELHandSlot.UpdateInventorySlot(invItem, 1);
			}
		}
		else if (!item && (m_pELHandSlot || m_pELHandSlotBackground))
		{
			if (m_pELHandSlot)
			{
				m_pELHandSlot.Destroy();
				m_pELHandSlot = null;
			}

			if (m_pELHandSlotBackground)
			{
				m_pELHandSlotBackground.RemoveFromHierarchy();
				m_pELHandSlotBackground = null;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	override void RefreshList()
	{
		super.RefreshList();
		EL_UpdateHandSlot();
	}

	//------------------------------------------------------------------------------------------------
	override int CreateSlots()
	{
		super.CreateSlots();

		for (int nSlot = 0; nSlot < m_aSlots.Count(); nSlot++)
		{
			SCR_InventorySlotUI pSlot = m_aSlots[nSlot];
			if (!pSlot) continue;
			InventoryItemComponent item = pSlot.GetInventoryItemComponent();
			if (!item) continue;
			InventoryStorageSlot itemSlot = item.GetParentSlot();
			if (itemSlot && EL_HandInventoryStorageComponent.Cast(itemSlot.GetStorage()))
			{
				pSlot.Destroy();
				m_aSlots.RemoveOrdered(nSlot--);
			}
		}

		return m_aSlots.Count() - 1;
	}
}
