[EntityEditorProps(category: "EveronLife/Gadgets", description: "Gadget that needs to be held in hand", color: "0 0 255 255")]
class EL_ForcedHandGadgetComponentClass: SCR_GadgetComponentClass
{
};

class EL_ForcedHandGadgetComponent : SCR_GadgetComponent
{
	private EGadgetMode m_LastMode;

	//------------------------------------------------------------------------------------------------
	private void ForceEquipToHand(IEntity player)
	{
		EL_Utils.ClearHands(player);
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(player.FindComponent(CharacterControllerComponent));
		//Play none gesture to stop pickup gesture
		//TODO: Add custom pickup / drop gesture
		characterController.TryPlayItemGesture(EItemGesture.EItemGestureNone);
		characterController.TakeGadgetInLeftHand(GetOwner(), EGadgetType.CONSUMABLE, false, true);
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

		RplComponent rplComponent = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rplComponent || !rplComponent.IsMaster())
			return;

		//Authority only

		//If moved from Hand to player inventory -> DROP
		if (m_LastMode == EGadgetMode.IN_HAND)
			EL_Utils.MoveToVicinity(charOwner, GetOwner());

		//If moved to player inventory -> EQUIP
		IEntity storageOwner = EL_Utils.GetStorageOwner(GetOwner());
		if (mode == EGadgetMode.IN_STORAGE && storageOwner == charOwner)
			ForceEquipToHand(charOwner);
	}

	//------------------------------------------------------------------------------------------------
	//! Force change mode to storage on action to trigger removal
	override protected void ActivateAction()
	{
		SCR_GadgetManagerComponent.GetGadgetManager(m_CharacterOwner).SetGadgetMode(GetOwner(), EGadgetMode.IN_STORAGE);
	}
};