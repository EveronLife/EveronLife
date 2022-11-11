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
		string displayQuantity;

		if (quantity >= 1000000000)
		{
			displayQuantity = string.Format("%1B", (quantity / 1000000000.0).ToString(-1, 1));
		}
		else if (quantity >= 1000000)
		{
			displayQuantity = string.Format("%1M", (quantity / 1000000.0).ToString(-1, 1));
		}
		else if (quantity >= 1000)
		{
			displayQuantity = string.Format("%1K", (quantity / 1000.0).ToString(-1, 1));
		}
		else
		{
			displayQuantity = quantity.ToString()
		}

		m_wStackNumber.SetText(displayQuantity);
		m_wStackNumber.SetVisible(m_widget.IsVisible() && (quantity > 1));
	}

	//------------------------------------------------------------------------------------------------
	void EL_SetLockState(bool state)
	{
		if (m_wItemLockThrobber) m_wItemLockThrobber.SetVisible(m_pItem && state);
	}
}
