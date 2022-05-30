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

		IEntity entity = quantityComponent.GetOwner();
		if (!entity)
		{
			return;
		}

		BaseInventoryStorageComponent storage = quantityComponent.GetOwningStorage();
		
		quantityComponent.Split(storage, m_InventoryManager);
	}

	override void NavigationBarUpdate()
	{
		super.NavigationBarUpdate();
		
		m_pNavigationBar.SetButtonEnabled("ButtonDropQuantity", true);
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
};
