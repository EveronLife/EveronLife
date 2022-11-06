[EntityEditorProps(category: "EveronLife/HandCarry", description: "Storage for items that can only be carried in hands", color: "0 0 255 255")]
class EL_HandInventoryStorageComponentClass: UniversalInventoryStorageComponentClass
{
}

enum EL_EHandCarryState
{
	NONE,
	AWAIT_HOLSTER,
	READY,
	ACTIVE
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

		CharacterControllerComponent characterController = EL_ComponentFinder<CharacterControllerComponent>.Find(GetOwner());

		m_eState = EL_EHandCarryState.READY;
		m_pNextCarryItem = item;

		// If gadget, remove gadget from hand (instantly, no need to wait for gadget change invoke)
		if (characterController.GetAttachedGadgetAtLeftHandSlot())
		{
			characterController.RemoveGadgetFromHand(true);
		}

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

		if (m_eState == EL_EHandCarryState.ACTIVE && (!isInHand || isOnGround))
		{
			// Stop current item carry because it was put back or dropped
			StopCarry();
		}
		else if(m_eState != EL_EHandCarryState.ACTIVE && isInHand)
		{
			// If active gadget is in the hand storage update the state to active
			InventoryItemComponent inventoryItem = EL_ComponentFinder<InventoryItemComponent>.Find(gadget);
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
	protected void StopCarry()
	{
		CharacterControllerComponent characterController = EL_ComponentFinder<CharacterControllerComponent>.Find(GetOwner());
		characterController.RemoveGadgetFromHand(true);
		ClearHandStorage(); // Drop all hand carry items (it should normally just be the one we currently carry)
		m_eState = EL_EHandCarryState.NONE;
	}

	//------------------------------------------------------------------------------------------------
	protected void ClearHandStorage(IEntity keepItem = null)
	{
		InventoryStorageManagerComponent inventoryManager = EL_ComponentFinder<InventoryStorageManagerComponent>.Find(GetOwner());
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