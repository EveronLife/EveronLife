modded class SCR_InventorySlotUI
{
	private EL_InventoryStackComponent m_EL_StackComponent;

	override void UpdateReferencedComponent(InventoryItemComponent pComponent)
	{
		super.UpdateReferencedComponent(pComponent);

		if (!pComponent)
		{
			return;
		}

		m_EL_StackComponent = EL_InventoryStackComponent.Cast(pComponent.GetOwner().FindComponent(EL_InventoryStackComponent));
		if (m_EL_StackComponent)
		{
			SetStackNumber(m_EL_StackComponent.GetQuantity());
		}
	}

	EL_InventoryStackComponent EL_GetInventoryStackComponent()
	{
		return m_EL_StackComponent;
	}
};
