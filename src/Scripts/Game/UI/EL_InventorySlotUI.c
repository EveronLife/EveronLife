modded class SCR_InventorySlotUI
{
	override void UpdateReferencedComponent(InventoryItemComponent pComponent)
	{
		super.UpdateReferencedComponent(pComponent);

		if (!pComponent)
		{
			return;
		}

		EL_InventoryStackComponent stackComponent = EL_InventoryStackComponent.Cast(pComponent.GetOwner().FindComponent(EL_InventoryStackComponent));
		if (stackComponent)
		{
			SetStackNumber(stackComponent.GetQuantity());
		}
	}
};
