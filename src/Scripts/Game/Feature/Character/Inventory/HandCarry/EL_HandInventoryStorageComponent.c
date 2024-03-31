[EntityEditorProps(category: "EveronLife/HandCarry", description: "Storage for items that can only be carried in hands", color: "0 0 255 255")]
class EL_HandInventoryStorageComponentClass: UniversalInventoryStorageComponentClass
{
}

enum EL_EHandCarryState
{
	NONE,
	AWAIT_HOLSTER,
	READY,
	ACTIVE,
	AWAIT_SWAP
}

class EL_HandInventoryStorageComponent : UniversalInventoryStorageComponent
{
	protected EL_EHandCarryState m_eState;
	protected IEntity m_pNextCarryItem;

	//------------------------------------------------------------------------------------------------
	override protected bool CanStoreItem(IEntity item, int slotID)
	{
		return item && item.FindComponent(EL_HandCarryComponent);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnAddedToSlot(IEntity item, int slotID)
	{
		super.OnAddedToSlot(item, slotID);

		if (!HasLocalControl()) return;

		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(GetOwner());

		IEntity currentGadget = characterController.GetAttachedGadgetAtLeftHandSlot();
		if (currentGadget)
		{
			if (currentGadget == item) return; //Handle same storage drag and drop during hand carry

			// If gadget, remove gadget from hand (instantly, no need to wait for gadget change invoke)
			characterController.RemoveGadgetFromHand(true);
		}

		m_pNextCarryItem = item;
		m_eState = EL_EHandCarryState.READY;

		// If weapon, holster weapon and and wait invoke of weapon changed
		if (characterController.GetWeaponManagerComponent().GetCurrent())
		{
			m_eState = EL_EHandCarryState.AWAIT_HOLSTER;
			characterController.SelectWeapon(null);
		}

		TakeToHands();
	}

	//------------------------------------------------------------------------------------------------
	void OnGadgetStateChanged(IEntity gadget, bool isInHand, bool isOnGround)
	{
		if (!HasLocalControl()) return;

		if (m_eState >= EL_EHandCarryState.ACTIVE && (!isInHand || isOnGround))
		{
			StopCarry(gadget);
		}
		else if(m_eState != EL_EHandCarryState.ACTIVE && isInHand)
		{
			// If active gadget is in the hand storage update the state to active
			InventoryItemComponent inventoryItem = EL_Component<InventoryItemComponent>.Find(gadget);
			if (inventoryItem)
			{
				InventoryStorageSlot parentSlot = inventoryItem.GetParentSlot();
				if (parentSlot && parentSlot.GetStorage() == this)
				{
					m_eState = EL_EHandCarryState.ACTIVE;
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	void OnGadgetModeSet(IEntity gadget, EGadgetMode targetMode, bool doFocus)
	{
		if (m_eState != EL_EHandCarryState.ACTIVE || !gadget || targetMode != EGadgetMode.IN_HAND) return;
		m_eState = EL_EHandCarryState.AWAIT_SWAP;
	}

	//------------------------------------------------------------------------------------------------
	void OnWeaponChanged(BaseWeaponComponent weapon, BaseWeaponComponent prevWeapon)
	{
		if (!HasLocalControl()) return;

		if(m_eState == EL_EHandCarryState.ACTIVE && weapon)
		{
			StopCarry();
		}
		else if (m_eState == EL_EHandCarryState.AWAIT_HOLSTER && !weapon)
		{
			m_eState = EL_EHandCarryState.READY;
			TakeToHands();
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void TakeToHands()
	{
		if (!m_pNextCarryItem || m_eState != EL_EHandCarryState.READY) return;

		IEntity character = GetOwner();

		// Remove all hand items except for the one that we want to carry
		ClearHandStorage(m_pNextCarryItem);

		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(character);
		if (gadgetManager) gadgetManager.HandleInput(m_pNextCarryItem, 1);

		m_pNextCarryItem = null;
	}

	//------------------------------------------------------------------------------------------------
	protected void StopCarry(IEntity gadget = null)
	{
		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(GetOwner());
		if (m_eState != EL_EHandCarryState.AWAIT_SWAP)
		{
			// Only remove gadget if there not a new one scheduled. Otherwise the new gadget will be removed instead "too".
			characterController.RemoveGadgetFromHand(true);
		}
		ClearHandStorage(); // Drop all hand carry items (it should normally just be the one we currently carry)
		if (gadget) EL_InventoryUtils.DropItem(GetOwner(), gadget); // Drop current gadget from different storage it might has been moved to
		m_eState = EL_EHandCarryState.NONE;
	}

	//------------------------------------------------------------------------------------------------
	protected void ClearHandStorage(IEntity keepItem = null)
	{
		InventoryStorageManagerComponent inventoryManager = EL_Component<InventoryStorageManagerComponent>.Find(GetOwner());
		array<IEntity> outItems();
		GetAll(outItems);
		foreach (IEntity handItem : outItems)
		{
			if (handItem != keepItem) inventoryManager.TryRemoveItemFromStorage(handItem, this);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool HasLocalControl()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		return playerController && GetOwner() == playerController.GetControlledEntity();
	}
}
