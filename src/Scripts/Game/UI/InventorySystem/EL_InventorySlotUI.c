modded class SCR_InventorySlotUI
{
	override void SetItemFunctionality()
	{
		super.SetItemFunctionality();
		
		EL_UpdateQuantityNumber();
	}
	
	override void UpdateStackNumber()
	{
		super.UpdateStackNumber();
		
		EL_UpdateQuantityNumber();
	}
	
	InventoryItemComponent EL_GetItemComponent()
	{
		return m_pItem;
	}
	
	EL_InventoryQuantityComponent EL_GetQuantityComponent()
	{
		if (!m_pItem)
		{
			return null;
		}
		
		IEntity owner = m_pItem.GetOwner();		
		return EL_InventoryQuantityComponent.Cast(owner.FindComponent(EL_InventoryQuantityComponent));
	}
		
	void EL_UpdateQuantityNumber()
	{		
		auto quantityComponent = EL_GetQuantityComponent();
		if (!quantityComponent || !m_wStackNumber)
		{
			return;
		}
		
		int iQuantity = quantityComponent.GetQuantity();
		if (iQuantity <= 1)
		{
			m_wStackNumber.SetVisible(false);
			return;
		}
		
		string sQuantity = "te" + iQuantity.ToString();
        if (iQuantity >= 1000000)
		{
        	sQuantity = string.Format("%1M", (iQuantity / 1000000.0).ToString(-1, 1));
		}
		else if (iQuantity >= 1000)
		{
			sQuantity = string.Format("%1K", (iQuantity / 1000.0).ToString(-1, 1));
		}
		
		m_wStackNumber.SetVisible(true);
		m_wStackNumber.SetText(sQuantity);
	}
};
