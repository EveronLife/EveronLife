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
		if (!quantityComponent.Split(storage, m_InventoryManager, null))
		{
			return;
		}

		ShowStoragesList();
		ShowAllStoragesInList();
		RefreshLootUIListener();
		RefreshUISlotStorages();
		NavigationBarUpdate();
	}

	override void NavigationBarUpdate()
	{
		super.NavigationBarUpdate();
		
		m_pNavigationBar.SetButtonEnabled("ButtonDropQuantity", true);
	}
};
