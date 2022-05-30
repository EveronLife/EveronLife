modded class SCR_InventorySlotUI
{
	private EL_InventoryQuantityComponent m_EL_QuantityComponent;

	override void UpdateReferencedComponent(InventoryItemComponent pComponent)
	{
		super.UpdateReferencedComponent(pComponent);

		if (!pComponent)
		{
			return;
		}

		m_EL_QuantityComponent = EL_InventoryQuantityComponent.Cast(pComponent.GetOwner().FindComponent(EL_InventoryQuantityComponent));
		if (m_EL_QuantityComponent)
		{
			SetQuantityNumber(m_EL_QuantityComponent.GetQuantity());
		}
	}

	EL_InventoryQuantityComponent EL_GetInventoryQuantityComponent()
	{
		return m_EL_QuantityComponent;
	}
};
