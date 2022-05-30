modded class SCR_NavigationBarUI
{
	override void HandlerAttached( Widget w )
	{
		EL_AddButton("EL_Inventory_SplitStack", "ButtonDropStack");
				
		super.HandlerAttached(w);
		
		EL_SetButtonName("ButtonDropStack", "Split Stack");
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

		button.SetLabel("Split Stack");
	}
};
