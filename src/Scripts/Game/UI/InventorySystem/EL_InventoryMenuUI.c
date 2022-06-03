modded class SCR_InventoryMenuUI
{
	override void OnAction(SCR_NavigationButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{
		switch (action)
		{
		case "EL_Inventory_SplitQuantity":
		{
			EL_SplitQuantity();
			return;
		}
		}

		super.OnAction(comp, action, pParentStorage, traverseStorageIndex);
	}

	bool EL_CanSplitQuantity()
	{
		if (!m_pFocusedSlotUI)
		{
			return false;
		}

		EL_InventoryQuantityComponent quantityComponent = m_pFocusedSlotUI.EL_GetInventoryQuantityComponent();
		if (!quantityComponent)
		{
			return false;
		}
		
		//! Don't show the dialog if there is nothing to split
		if (quantityComponent.GetQuantity() <= 1)
		{
			return false;
		}

		IEntity entity = quantityComponent.GetOwner();
		if (!entity)
		{
			return false;
		}

		return true;
	}

	void EL_SplitQuantity()
	{
		if (!m_pFocusedSlotUI)
		{
			return;
		}

		EL_InventoryQuantityComponent quantityComponent = m_pFocusedSlotUI.EL_GetInventoryQuantityComponent();
		if (!quantityComponent)
		{
			return;
		}
		
		//! Don't show the dialog if there is nothing to split
		if (quantityComponent.GetQuantity() <= 1)
		{
			return;
		}

		IEntity entity = quantityComponent.GetOwner();
		if (!entity)
		{
			return;
		}
		
		EL_QuantityDialog menu = EL_QuantityDialog.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.EL_QuantityDialog, DialogPriority.INFORMATIVE, 0, true));
		if (!menu)
		{
			return;
		}

		menu.m_InventoryManager = m_InventoryManager;
		menu.m_OwningStorage = quantityComponent.GetOwningStorage();
		menu.m_QuantityComponent = quantityComponent;
		
		menu.OnRefresh();
	}

	override void NavigationBarUpdate()
	{
		super.NavigationBarUpdate();
		
		m_pNavigationBar.SetButtonEnabled("ButtonDropQuantity", EL_CanSplitQuantity());
	}

	void EL_Refresh()
	{
		if (m_pVicinity)
		{
			m_pVicinity.ManipulationComplete();
		}

		ShowStoragesList();
		ShowAllStoragesInList();

		RefreshLootUIListener();
		RefreshUISlotStorages();
		NavigationBarUpdate();
	}

	SCR_InventorySlotUI EL_GetFocusedSlotUI()
	{
		return m_pFocusedSlotUI;
	}

	SCR_InventorySlotUI EL_GetSelectedSlotUI()
	{
		return m_pSelectedSlotUI;
	}

	SCR_InventorySlotUI EL_GetPrevSelectedSlotUI()
	{
		return m_pPrevSelectedSlotUI;
	}

	static void EL_PrintSlotUI(SCR_InventorySlotUI slot)
	{
		Print(slot);
		if (!slot)
		{
			return;
		}

		Print(slot.EL_GetInventoryQuantityComponent());
	}
};