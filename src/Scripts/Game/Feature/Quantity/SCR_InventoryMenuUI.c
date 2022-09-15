modded class SCR_InventoryMenuUI
{
	protected SCR_InventorySlotUI m_pELSelectedQuantitySlot;
	protected bool m_bELKeepQuantitySeperate;

	protected ref set<IEntity> m_aELRefreshEntities = new set<IEntity>();
	protected SCR_InventorySlotUI m_aELQuantityOperationSourceSlotUi;
	protected SCR_InventorySlotUI m_aELQuantityOperationDestinationSlotUi;
	protected bool m_bELQuantityRefreshBlock;

	//------------------------------------------------------------------------------------------------
	override void MoveItemToStorageSlot()
	{
		bool skipMove;
		InventoryItemComponent itemSource;
		EL_QuantityComponent quantitySource;
		if(m_pSelectedSlotUI)
		{
			itemSource = m_pSelectedSlotUI.GetInventoryItemComponent();
			if (itemSource) quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(itemSource.GetOwner());
		}

		if (quantitySource)
		{
			InventoryItemComponent itemDestination;
			EL_QuantityComponent quantityDestination;
			if (m_pFocusedSlotUI)
			{
				itemDestination = m_pFocusedSlotUI.GetInventoryItemComponent();
				if (itemDestination) quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(itemDestination.GetOwner());
			}

			if (quantityDestination)
			{
				if (quantityDestination.CanCombine(quantitySource))
				{
					m_InventoryManager.EL_RequestQuantityTransfer(quantitySource.GetOwner(), quantityDestination.GetOwner());
					skipMove = true; // Only quantity transfer, no actual inventory operation
				}
				else
				{
					return;
				}
			}
			else
			{
				m_InventoryManager.EL_SetTransferIntent(quantitySource.GetOwner(), m_bELKeepQuantitySeperate);
			}

			m_aELQuantityOperationSourceSlotUi = m_pSelectedSlotUI;
			m_aELQuantityOperationDestinationSlotUi = m_pFocusedSlotUI;
		}

		if (skipMove)
		{
			// "Fake" the UI experience as if it was a real inventory operation
			SCR_UISoundEntity.SoundEvent("SOUND_INV_HOTKEY_CONFIRM");
			m_pSelectedSlotUI.EL_SetLockState(true);
			m_pSelectedSlotUI.SetSelected(false);
		}
		else
		{
			super.MoveItemToStorageSlot();
		}

		EL_ResetQuantitySelection();
	}

	//------------------------------------------------------------------------------------------------
	override void MoveItem(SCR_InventoryStorageBaseUI pStorageBaseUI = null)
	{
		bool skipMove;
		IEntity transferEntity;
		if (m_pSelectedSlotUI)
		{
			InventoryItemComponent item = m_pSelectedSlotUI.GetInventoryItemComponent();
			if (item) transferEntity = item.GetOwner();
		}
		if (transferEntity && transferEntity.FindComponent(EL_QuantityComponent))
		{
			BaseInventoryStorageComponent targetStorage = m_pActiveHoveredStorageUI.GetCurrentNavigationStorage();
			if (targetStorage)
			{
				m_InventoryManager.EL_SetTransferIntent(transferEntity, m_bELKeepQuantitySeperate);
				m_aELQuantityOperationSourceSlotUi = m_pSelectedSlotUI;
				m_aELQuantityOperationDestinationSlotUi = null;
			}
		}

		super.MoveItem(pStorageBaseUI);

		EL_ResetQuantitySelection();
	}

	//------------------------------------------------------------------------------------------------
	override protected void MoveBetweenFromVicinity()
	{
		if (m_bELKeepQuantitySeperate)
		{
			IEntity transferEntity;
			if (m_pSelectedSlotUI)
			{
				InventoryItemComponent item = m_pSelectedSlotUI.GetInventoryItemComponent();
				if (item) transferEntity = item.GetOwner();
			}
			if (transferEntity && transferEntity.FindComponent(EL_QuantityComponent))
			{
				Print(transferEntity);
				m_InventoryManager.EL_SetTransferIntent(transferEntity, true);
			}
		}

		super.MoveBetweenFromVicinity();
	}

	//------------------------------------------------------------------------------------------------
	void EL_ResetQuantitySelection()
	{
		m_pELSelectedQuantitySlot = null;
		if(m_bStorageSwitchMode) SetStorageSwitchMode(false);
		NavigationBarUpdate();
	}

	//------------------------------------------------------------------------------------------------
	void EL_Refresh(IEntity refreshEntity = null)
	{
		m_bELQuantityRefreshBlock = true;
		if (refreshEntity)
		{
			if (m_aELQuantityOperationSourceSlotUi && m_aELQuantityOperationSourceSlotUi.GetInventoryItemComponent().GetOwner() != refreshEntity ||
				m_aELQuantityOperationDestinationSlotUi && m_aELQuantityOperationDestinationSlotUi.GetInventoryItemComponent().GetOwner() != refreshEntity)
			{
				m_aELRefreshEntities.Insert(refreshEntity);
			}
		}
		ScriptCallQueue queue = GetGame().GetCallqueue();
		queue.Remove(EL_RefreshDebounced);
		queue.CallLater(EL_RefreshDebounced, 10);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_RefreshDebounced()
	{
		m_bELQuantityRefreshBlock = false;
		bool fullRefresh = !m_aELQuantityOperationSourceSlotUi && !m_aELQuantityOperationDestinationSlotUi;

		if (!m_aELRefreshEntities.IsEmpty())
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
					if (item && m_aELRefreshEntities.Contains(item.GetOwner()))
					{
						storage.Refresh();
						break;
					}
				}
			}

			m_aELRefreshEntities.Clear();
			fullRefresh = false;
		}

		if (m_aELQuantityOperationSourceSlotUi)
		{
			IEntity selectedEntity = m_aELQuantityOperationSourceSlotUi.GetInventoryItemComponent().GetOwner();
			SCR_InventoryStorageBaseUI storageUi = m_aELQuantityOperationSourceSlotUi.GetStorageUI();
			storageUi.Refresh();
			if (IsUsingGamepad())
			{
				bool focusSet;
				if (selectedEntity)
				{
					foreach(SCR_InventorySlotUI slot : storageUi.GetUISlots())
					{
						InventoryItemComponent item = slot.GetInventoryItemComponent();
						if (item && item.GetOwner() == selectedEntity)
						{
							GetGame().GetWorkspace().SetFocusedWidget(slot.GetButtonWidget());
							focusSet = true;
							break;
						}
					}
				}

				if (!focusSet) FocusOnSlotInStorage(storageUi);
			}
		}
		if (m_aELQuantityOperationDestinationSlotUi) m_aELQuantityOperationDestinationSlotUi.GetStorageUI().Refresh();

		if (fullRefresh)
		{
			// General refresh
			if (m_pVicinity) m_pVicinity.ManipulationComplete();
			ShowStoragesList();
			ShowAllStoragesInList();
			RefreshLootUIListener();
		}

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
	override void NavigationBarUpdateGamepad()
	{
		if (m_pSelectedSlotUI)
		{
			EL_QuantityComponent quantitySource;
			InventoryItemComponent itemSource = m_pSelectedSlotUI.GetInventoryItemComponent();
			if (itemSource) quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(itemSource.GetOwner());

			m_pNavigationBar.SetAllButtonEnabled(false);

			bool isFocusStorage = !m_pFocusedSlotUI || (m_pFocusedSlotUI && m_pFocusedSlotUI.Type().IsInherited(SCR_InventorySlotStorageEmbeddedUI));
			bool isValidTargetStorage = isFocusStorage && m_pActiveStorageUI && !EL_Utils.IsAnyInherited(m_pActiveStorageUI, {SCR_InventoryStoragesListUI, SCR_InventoryStorageWeaponsUI});
			bool canTransfer = m_pActiveStorageUI && m_pSelectedSlotUI && (m_pActiveStorageUI != m_pSelectedSlotUI.GetStorageUI());

			EL_QuantityComponent quantityDestination;
			if (m_pFocusedSlotUI)
			{
				InventoryItemComponent itemDestination = m_pFocusedSlotUI.GetInventoryItemComponent();
				if (itemDestination) quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(itemDestination.GetOwner());
			}

			if ((isValidTargetStorage && canTransfer) || quantityDestination && quantityDestination.CanCombine(quantitySource))
			{
				m_pNavigationBar.SetButtonEnabled("ButtonSelect", true);
				m_pNavigationBar.SetButtonActionName("ButtonSelect", "Inventory_Move");
			}

			if (isValidTargetStorage)
			{
				m_pNavigationBar.SetButtonEnabled("ButtonOpenStorage", true);
				if (canTransfer) m_pNavigationBar.SetButtonEnabled("EL_ButtonTransferStack", true);
			}

			return;
		}
		else if (m_pFocusedSlotUI && m_pActiveStorageUI && m_pActiveStorageUI.Type().IsInherited(SCR_InventoryStorageLootUI))
		{
			InventoryItemComponent itemSource = m_pFocusedSlotUI.GetInventoryItemComponent();
			if (itemSource && itemSource.GetOwner().FindComponent(EL_QuantityComponent))
			{
				super.NavigationBarUpdateGamepad();
				m_pNavigationBar.SetButtonEnabled("EL_ButtonTransferStack", true);
				return;
			}
		}

		super.NavigationBarUpdateGamepad();
	}

	//------------------------------------------------------------------------------------------------
	override void OnAction(SCR_NavigationButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{
		if (action == "EL_Inventory_TransferStack")
		{
			m_bELKeepQuantitySeperate = true;
			if (m_pELSelectedQuantitySlot)
			{
				Action_Drop();
			}
			else
			{
				Action_MoveBetween();
			}
		}
		else if (m_pELSelectedQuantitySlot)
		{
			switch (action)
			{
				case "Inventory_Select":
				{
					Action_Drop();
					return;
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

		// Fix selected slot being overriden by TraverseActualSlot() on controller
		if (!m_pSelectedSlotUI && m_pELSelectedQuantitySlot)
		{
			m_pSelectedSlotUI = m_pELSelectedQuantitySlot;
		}
	}

	//------------------------------------------------------------------------------------------------
	override protected void SimpleFSM(EMenuAction EAction = EMenuAction.ACTION_SELECT)
	{
		super.SimpleFSM(EAction);

		if (EAction != EMenuAction.ACTION_SELECT || !IsUsingGamepad() || !m_pSelectedSlotUI) return;

		InventoryItemComponent itemSource = m_pSelectedSlotUI.GetInventoryItemComponent();
		if (!itemSource) return;

		if (!itemSource.GetOwner().FindComponent(EL_QuantityComponent)) return;
		m_pELSelectedQuantitySlot = m_pSelectedSlotUI;
		SetStorageSwitchMode(true);
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

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();

		GetGame().GetInputManager().AddActionListener("EL_Inventory_TransferStack", EActionTrigger.UP, EL_OnTransferStackPressed);
		GetGame().GetInputManager().AddActionListener("EL_Inventory_TransferStack", EActionTrigger.DOWN, EL_OnTransferStackPressed);
	}

	//------------------------------------------------------------------------------------------------
	override protected void Action_CloseInventory()
	{
		super.Action_CloseInventory();

		GetGame().GetInputManager().AddActionListener("EL_Inventory_TransferStack", EActionTrigger.UP, EL_OnTransferStackPressed);
		GetGame().GetInputManager().AddActionListener("EL_Inventory_TransferStack", EActionTrigger.DOWN, EL_OnTransferStackPressed);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_OnTransferStackPressed(float value, EActionTrigger reason)
	{
		m_bELKeepQuantitySeperate = value >= 0.9;
	}
}
