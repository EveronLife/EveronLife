[EntityEditorProps(category: "EveronLife/Gadgets", description: "Gadget that needs to be held in hand", color: "0 0 255 255")]
class EL_ForcedHandGadgetComponentClass: SCR_GadgetComponentClass
{
};

class EL_ForcedHandGadgetComponent : SCR_GadgetComponent
{
	private EGadgetMode m_LastMode;
	private bool m_bDelete;

	//------------------------------------------------------------------------------------------------
	//! Mark the gadget for deletion and set the gadget mode to IN_STORAGE
	void Delete()
	{
		m_bDelete = true;
		SCR_GadgetManagerComponent.GetGadgetManager(m_CharacterOwner).SetGadgetMode(GetOwner(), EGadgetMode.IN_STORAGE);
	}

	//------------------------------------------------------------------------------------------------
	override void ModeClear(EGadgetMode mode)
	{
		m_LastMode = mode;
		super.ModeClear(mode);
	}

	//------------------------------------------------------------------------------------------------
	override void ModeSwitch(EGadgetMode mode, IEntity charOwner)
	{
		super.ModeSwitch(mode, charOwner);
		IEntity controlledEnt = SCR_PlayerController.GetLocalControlledEntity();
		if ( !controlledEnt || controlledEnt != charOwner )
		{
			return;
		}

		//Owner only:

		//Delete only if IN_STORAGE to reset player speed modifier
		if (m_bDelete && mode == EGadgetMode.IN_STORAGE)
		{
			InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(charOwner.FindComponent(SCR_InventoryStorageManagerComponent));
			inventoryManager.TryDeleteItem(GetOwner());
			return;
		}

		//If moved from Hand to player inventory -> DROP
		if (m_LastMode == EGadgetMode.IN_HAND)
			EL_Utils.MoveToVicinity(charOwner, GetOwner());

		//If moved to player inventory -> EQUIP
		IEntity storageOwner = EL_Utils.GetStorageOwner(GetOwner());
		if (mode == EGadgetMode.IN_STORAGE && storageOwner == charOwner)
			EL_Utils.ForceEquipToHand(charOwner, GetOwner());
	}

	//------------------------------------------------------------------------------------------------
	//! Force change mode to storage on action to trigger removal
	override protected void ActivateAction()
	{
		SCR_GadgetManagerComponent.GetGadgetManager(m_CharacterOwner).SetGadgetMode(GetOwner(), EGadgetMode.IN_STORAGE);
	}
};