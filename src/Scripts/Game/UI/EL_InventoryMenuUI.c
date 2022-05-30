modded class SCR_InventoryMenuUI
{
	override void OnAction(SCR_NavigationButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{
		switch (action)
		{
		case "EL_Inventory_SplitStack":
		{
			EL_SplitStack();
			return;
		}
		}

		super.OnAction(comp, action, pParentStorage, traverseStorageIndex);
	}

	void EL_SplitStack()
	{
		if (!m_pFocusedSlotUI)
		{
			return;
		}

		EL_InventoryStackComponent stackComponent = m_pFocusedSlotUI.EL_GetInventoryStackComponent();
		if (!stackComponent)
		{
			return;
		}

		IEntity entity = stackComponent.GetOwner();
		if (!entity)
		{
			return;
		}

		BaseInventoryStorageComponent storage = stackComponent.GetOwningStorage();
		if (!stackComponent.Split(storage, m_InventoryManager, null))
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
		
		m_pNavigationBar.SetButtonEnabled("ButtonDropStack", true);
	}
};
