class EL_QuantityDialog : DialogUI // why no prefix, this aint generated you fuck
{
	BaseInventoryStorageComponent m_OwningStorage;
	EL_InventoryQuantityComponent m_QuantityComponent;
	SCR_InventoryStorageManagerComponent m_InventoryManager;
	
	SCR_NavigationButtonComponent m_DecreaseQuantity;
	SCR_NavigationButtonComponent m_IncreaseQuantity;
	
	int m_QuantityValue;
	
	int m_MinimumQuantityValue;
	int m_MaximumQuantityValue;
	
	EditBoxWidget m_wQuantityInput;
	TextWidget m_wQuantityMax;

	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		Widget w = GetRootWidget();

		// Cancel button
		m_DecreaseQuantity = SCR_NavigationButtonComponent.GetNavigationButtonComponent("DecreaseButton", w);
		if (m_DecreaseQuantity)
		{
			m_DecreaseQuantity.m_OnActivated.Insert(OnDecrease);
		}
		
		// Confirm button
		m_IncreaseQuantity = SCR_NavigationButtonComponent.GetNavigationButtonComponent("IncreaseButton", w);
		if (m_IncreaseQuantity)
		{
			m_IncreaseQuantity.m_OnActivated.Insert(OnIncrease);
		}
		
		m_wQuantityInput = EditBoxWidget.Cast(w.FindAnyWidget("QuantityInput"));
		m_wQuantityMax = TextWidget.Cast(w.FindAnyWidget("QuantityMax"));

		OnRefresh();
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_wQuantityInput)
		{
			string quantityValueStr = m_wQuantityInput.GetText();
			
			if (quantityValueStr.Length() == 0)
			{
				m_wQuantityInput.SetText("");
				m_QuantityValue = -1;
			}
			else
			{
				m_QuantityValue = quantityValueStr.ToInt();
			}
			
			OnRefresh();
			return true;
		}
		
		return super.OnChange(w, x, y, finished);
	}
	
	void OnRefresh()
	{
		if (!m_QuantityComponent)
		{
			return;
		}
		
		//! Minimum split value is 1, can't remove nothing. 
		//!   If the user wants to split nothing they should click cancel instead
		m_MinimumQuantityValue = 1;
		
		//! Maximum split value is the current quantity minus 1, to allow atleast 1 item to be split
		m_MaximumQuantityValue = m_QuantityComponent.GetQuantity() - 1;
		
		if (m_QuantityValue == -1)
		{
			return;
		}
		
		if (m_QuantityValue < m_MinimumQuantityValue)
		{
			m_QuantityValue = m_MinimumQuantityValue;
		}
		
		if (m_QuantityValue > m_MaximumQuantityValue)
		{
			m_QuantityValue = m_MaximumQuantityValue;
		}
		
		m_wQuantityInput.SetText(m_QuantityValue.ToString());
		m_wQuantityMax.SetText((m_MaximumQuantityValue + 1).ToString());
	}

	void OnDecrease()
	{
		if (m_QuantityValue == -1)
		{
			m_QuantityValue = m_MinimumQuantityValue;
		}
		
		m_QuantityValue--;
		
		OnRefresh();
	}
	
	void OnIncrease()
	{
		if (m_QuantityValue == -1)
		{
			m_QuantityValue = m_MaximumQuantityValue;
		}
		
		m_QuantityValue++;
		
		OnRefresh();
	}
		
	override void OnConfirm()
	{
		super.OnConfirm();
		
		float split = m_QuantityValue / (m_MaximumQuantityValue + 1);
		m_QuantityComponent.Split(m_OwningStorage, split, m_InventoryManager);
	}
};
