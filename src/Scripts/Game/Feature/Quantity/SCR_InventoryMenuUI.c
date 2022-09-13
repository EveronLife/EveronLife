modded class SCR_InventoryMenuUI
{
	protected ref set<IEntity> m_aELRefreshSources = new set<IEntity>();
	protected bool m_bELQuantityRefreshBlock;
	protected SCR_InventorySlotUI m_pELSelectedQuantitySlot;

	//------------------------------------------------------------------------------------------------
	override void MoveItemToStorageSlot()
	{
		EL_FixSelection();

		InventoryItemComponent itemDestination;
		EL_QuantityComponent quantityDestionation;
		if (m_pFocusedSlotUI)
		{
			/*
			auto storageSlot = SCR_InventorySlotStorageUI.Cast(m_pFocusedSlotUI);
			if (storageSlot)
			{
				SCR_InventoryStorageBaseUI storageUi = storageSlot.GetStorageUI();
				if (storageUi)
				{
					//Target slot was no combineable entity but a storage slot, so move the item into it
					MoveItem(storageUi);
					return;
				}
			}
			*/

			itemDestination = m_pFocusedSlotUI.GetInventoryItemComponent();
			if (itemDestination) quantityDestionation = EL_ComponentFinder<EL_QuantityComponent>.Find(itemDestination.GetOwner());
		}


		InventoryItemComponent itemSource;
		EL_QuantityComponent quantitySource;
		if(m_pSelectedSlotUI)
		{
			itemSource = m_pSelectedSlotUI.GetInventoryItemComponent();
			if (itemSource) quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(itemSource.GetOwner());
		}

		if (quantitySource && quantityDestionation && quantityDestionation.CanCombine(quantitySource))
		{
			if (quantitySource.GetQuantity() > quantityDestionation.GetRemainingCapacity())
			{
				m_InventoryManager.EL_RequestQuantityTransfer(quantitySource.GetOwner(), quantityDestionation.GetOwner());
				SCR_UISoundEntity.SoundEvent("SOUND_INV_HOTKEY_CONFIRM");
				return; // Purely scripted transfer, no actualy inventory operation takes place
			}
			else
			{
				m_InventoryManager.EL_SetQuantityTargetIntent(quantitySource.GetOwner(), quantityDestionation.GetOwner());
			}
		}
		else if (itemSource && itemDestination)
		{
			InventoryStorageSlot sourceSlot = itemSource.GetParentSlot();
			BaseInventoryStorageComponent sourceStorage;
			if (sourceSlot) sourceStorage = sourceSlot.GetStorage();

			InventoryStorageSlot destinationSlot = itemDestination.GetParentSlot();
			BaseInventoryStorageComponent destinationStorage;
			if (destinationSlot) destinationStorage = destinationSlot.GetStorage();

			// Abort action if origin and target storage are the same and no quantity change was possible
			// Not really needed but fixes vanilla AR behavior that forgot to cover this scenario
			if (sourceStorage == destinationStorage) return;
		}

		EL_ResetQuantitySelection();

		super.MoveItemToStorageSlot();
	}

	//------------------------------------------------------------------------------------------------
	override void MoveItem(SCR_InventoryStorageBaseUI pStorageBaseUI = null)
	{
		EL_FixSelection();

		// Drag and drop onto a selected target storage transfers stack as is
		IEntity transferEntity;
		if (m_pSelectedSlotUI)
		{
			InventoryItemComponent item = m_pSelectedSlotUI.GetInventoryItemComponent();
			if (item) transferEntity = item.GetOwner();
		}

		BaseInventoryStorageComponent targetStorage = m_pActiveHoveredStorageUI.GetCurrentNavigationStorage();
		if (transferEntity && targetStorage) m_InventoryManager.EL_SetQuantityTargetIntent(transferEntity, transferEntity);

		EL_ResetQuantitySelection();

		super.MoveItem(pStorageBaseUI);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_FixSelection()
	{
		//m_pSelectedSlotUI can be lost when opening nested storages. So we recover the info from the inital selection we remember
		if (!m_pSelectedSlotUI) m_pSelectedSlotUI = m_pELSelectedQuantitySlot;
	}

	//------------------------------------------------------------------------------------------------
	void EL_ResetQuantitySelection()
	{
		if (!m_pELSelectedQuantitySlot) return;
		m_pELSelectedQuantitySlot = null;
		SetStorageSwitchMode(false);
		NavigationBarUpdateGamepad();
	}

	//------------------------------------------------------------------------------------------------
	void EL_Refresh(IEntity refreshSource = null)
	{
		m_bELQuantityRefreshBlock = true;
		if (refreshSource) m_aELRefreshSources.Insert(refreshSource);
		ScriptCallQueue queue = GetGame().GetCallqueue();
		queue.Remove(EL_RefreshDebounced);
		queue.CallLater(EL_RefreshDebounced, 10);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_RefreshDebounced()
	{
		m_bELQuantityRefreshBlock = false;

		// Refresh only specific ui elements affected the by source entities
		if (!m_aELRefreshSources.IsEmpty())
		{
			array<SCR_InventoryStorageBaseUI> allStorages();
			allStorages.InsertAll(m_aStorages);
			if(m_pStorageLootUI) allStorages.Insert(m_pStorageLootUI);

			foreach (SCR_InventoryStorageBaseUI storage : allStorages)
			{
				if (!storage) continue;

				foreach(SCR_InventorySlotUI slot : storage.GetUISlots())
				{
					if (!slot.GetWidget().IsVisible()) continue;
					InventoryItemComponent item = slot.GetInventoryItemComponent();
					if (item && m_aELRefreshSources.Contains(item.GetOwner()))
					{
						storage.Refresh();
						break;
					}
				}
			}

			m_aELRefreshSources.Clear();
			return;
		}

		// General refresh
		if (m_pVicinity) m_pVicinity.ManipulationComplete();
		ShowStoragesList();
		ShowAllStoragesInList();
		RefreshLootUIListener();
		NavigationBarUpdate();
	}

	//------------------------------------------------------------------------------------------------
	override void ShowStoragesList()
	{
		if (!m_bELQuantityRefreshBlock) super.ShowStoragesList();
	}

	//------------------------------------------------------------------------------------------------
	override void ShowAllStoragesInList()
	{
		if (!m_bELQuantityRefreshBlock) super.ShowAllStoragesInList();
	}

	//------------------------------------------------------------------------------------------------
	override void RefreshLootUIListener()
	{
		if (!m_bELQuantityRefreshBlock) super.RefreshLootUIListener();
	}

	//------------------------------------------------------------------------------------------------
	override void NavigationBarUpdate()
	{
		if (!m_bELQuantityRefreshBlock) super.NavigationBarUpdate();
	}

	//------------------------------------------------------------------------------------------------
	override void OnAction( SCR_NavigationButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1 )
	{
		if (m_pELSelectedQuantitySlot)
		{
			switch (action)
			{
				case "Inventory_Select":
				{
					m_bStorageSwitchMode = true;
					action = "Inventory_Move";
					break;
				}

				case "Inventory_OpenStorage":
				{
					if (!m_pFocusedSlotUI || !m_pFocusedSlotUI.Type().IsInherited(SCR_InventorySlotStorageEmbeddedUI))
					{
						action = "Inventory_Select";
					}

					break;
				}
			}
		}

		super.OnAction(comp, action, pParentStorage, traverseStorageIndex);
	}

	//------------------------------------------------------------------------------------------------
	override void NavigationBarUpdateGamepad()
	{
		if (m_pELSelectedQuantitySlot)
		{
			m_pNavigationBar.SetAllButtonEnabled(false);

			bool isOpenableStorage = !m_pFocusedSlotUI || (m_pFocusedSlotUI && m_pFocusedSlotUI.Type().IsInherited(SCR_InventorySlotStorageEmbeddedUI));

			EL_QuantityComponent quantitySource;
			InventoryItemComponent itemsource = m_pELSelectedQuantitySlot.GetInventoryItemComponent();
			if (itemsource) quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(itemsource.GetOwner());

			EL_QuantityComponent quantityDestination;
			if (m_pFocusedSlotUI)
			{
				InventoryItemComponent itemDestination = m_pFocusedSlotUI.GetInventoryItemComponent();
				if (itemDestination) quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(itemDestination.GetOwner());
			}

			if (isOpenableStorage || quantityDestination && quantityDestination.CanCombine(quantitySource))
			{
				m_pNavigationBar.SetButtonEnabled("ButtonSelect", true);
				m_pNavigationBar.SetButtonActionName("ButtonSelect", "Inventory_Move");
			}

			if (isOpenableStorage) m_pNavigationBar.SetButtonEnabled("ButtonOpenStorage", true);

			return;
		}

		super.NavigationBarUpdateGamepad();
	}


	//------------------------------------------------------------------------------------------------
	override protected void SimpleFSM(EMenuAction EAction = EMenuAction.ACTION_SELECT)
	{
		super.SimpleFSM(EAction);

		if (!m_pSelectedSlotUI) return;

		InventoryItemComponent itemSource = m_pSelectedSlotUI.GetInventoryItemComponent();
		if (!itemSource) return;

		if (!itemSource.GetOwner().FindComponent(EL_QuantityComponent)) return;
		m_pELSelectedQuantitySlot = m_pSelectedSlotUI;
	}

	//------------------------------------------------------------------------------------------------
	override bool FocusOnSlotInStorage(SCR_InventoryStorageBaseUI storage, int id = 0, bool focus = true)
	{
		array<SCR_InventorySlotUI> slots = storage.GetUISlots();
		int currentPage = storage.GetLastShownPage();

		if (slots && id < slots.Count() && slots.Get(id).GetPage() != currentPage)
		{
			for (int nSlot = 0, slotCount = slots.Count(); nSlot < slotCount; nSlot++)
			{
				if (slots.Get(nSlot).GetPage() == currentPage)
				{
					id = nSlot;
					break;
				}
			}
		}

		return super.FocusOnSlotInStorage(storage, id, focus);
	}
}
