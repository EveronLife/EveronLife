modded class SCR_GadgetManagerComponent
{
	//------------------------------------------------------------------------------------------------
	override void SetGadgetMode(IEntity gadget, EGadgetMode targetMode, bool doFocus = false)
	{
		EL_HandInventoryStorageComponent handStorage = EL_Component<EL_HandInventoryStorageComponent>.Find(GetOwner());
		if (handStorage) handStorage.OnGadgetModeSet(gadget, targetMode, doFocus);
		super.SetGadgetMode(gadget, targetMode, doFocus);
	}
}
