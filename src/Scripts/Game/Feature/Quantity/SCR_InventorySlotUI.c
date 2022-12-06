modded class SCR_InventorySlotUI
{
	//------------------------------------------------------------------------------------------------
	override void SetSlotVisible(bool bVisible)
	{
		super.SetSlotVisible(bVisible);
		UpdateStackNumber();
	}

	//------------------------------------------------------------------------------------------------
	override protected void UpdateStackNumber()
	{
		EL_QuantityComponent quantityComponent;
		if (m_wStackNumber && m_pItem) quantityComponent = EL_QuantityComponent.Cast(m_pItem.GetOwner().FindComponent(EL_QuantityComponent));
		if (!quantityComponent)
		{
			super.UpdateStackNumber();
			return;
		}

		int quantity = quantityComponent.GetQuantity();
		m_wStackNumber.SetText(EL_FormatUtils.AbbreviateNumber(quantity));
		m_wStackNumber.SetVisible(m_widget.IsVisible() && (quantity > 1));
	}

	//------------------------------------------------------------------------------------------------
	void EL_SetLockState(bool state)
	{
		if (m_wItemLockThrobber) m_wItemLockThrobber.SetVisible(m_pItem && state);
	}
}
