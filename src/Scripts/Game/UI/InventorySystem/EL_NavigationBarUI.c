modded class SCR_NavigationBarUI
{
	override void HandlerAttached( Widget w )
	{
		EL_AddButton("EL_Inventory_SplitQuantity", "ButtonDropQuantity");
				
		super.HandlerAttached(w);
		
		EL_SetButtonName("ButtonDropQuantity", "Split Quantity");
	}

	void EL_AddButton(string action, string buttonId)
	{
		NavigationButtonEntry entry();
		entry.m_sAction = action;
		entry.m_sButtonID = buttonId;
		
		m_aEntries.Insert(entry);
	}

	void EL_SetButtonName(string buttonId, string name)
	{
		SCR_NavigationButtonComponent button = GetButton(buttonId);
		if (!button)
		{
			return;
		}

		button.SetLabel("Split Quantity");
	}
};
