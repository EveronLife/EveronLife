modded class SCR_NavigationBarUI
{
	//------------------------------------------------------------------------------------------------
	override void SetButtonEnabled(string sButtonName, bool bEnable = true, string sName = "")
	{
		if (sName.StartsWith("#")) //Workaround until https://feedback.bistudio.com/T167518 is implemented
		{
			SCR_NavigationButtonComponent pActionButton = GetButton(sButtonName);
			if(!pActionButton) return;
			pActionButton.SetEnabled(bEnable);
			pActionButton.GetRootWidget().SetVisible(bEnable);
			pActionButton.SetLabel(sName);
			return;
		}

		super.SetButtonEnabled(sButtonName, bEnable, sName);
	}
}
