modded enum ChimeraMenuPreset {
    EL_VehicleShop
}

class EL_VehicleShopUI: ChimeraMenuBase
{
    Widget m_wRoot;
	SliderWidget m_RedSlider, m_GreenSlider, m_BlueSlider, m_wHandlingSlider, m_wTopSpeedSlider, m_wBreakingSlider, m_wAccelerationSlider, m_wInventorySizeSlider;
	TextWidget m_RedIndex, m_GreenIndex, m_BlueIndex, m_wVehiclePriceText, m_wVehicleTitleText;
	int r,g,b;
	ref Color m_NewColor = new Color();
	
	ref ScriptInvoker<int> m_OnVehicleSelectionChanged = new ScriptInvoker();
	ref ScriptInvoker<ref Color> m_OnColorChange = new ScriptInvoker();
	ref ScriptInvoker m_OnBuyVehicle = new ScriptInvoker();
	ref ScriptInvoker m_OnExit = new ScriptInvoker();
	
	Color GetCurrentSliderColor()
	{
		int newR = m_RedSlider.GetCurrent();
		int newG = m_GreenSlider.GetCurrent();
		int newB = m_BlueSlider.GetCurrent();
		return Color.FromRGBA(newR,newG,newB,255);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnApplyColor()
	{
		m_OnColorChange.Invoke(GetCurrentSliderColor());
	}		
	
	//------------------------------------------------------------------------------------------------
	void OnMenuLeft()
	{
		m_OnVehicleSelectionChanged.Invoke(-1);
	}		
	//------------------------------------------------------------------------------------------------
	void OnMenuRight()
	{
		m_OnVehicleSelectionChanged.Invoke(1);
	}	
	
	//------------------------------------------------------------------------------------------------
	void OnBuyVehicle()
	{
		m_OnBuyVehicle.Invoke();
		
	}	
	//------------------------------------------------------------------------------------------------
	void InvokeOnMenuClose()
	{
		m_OnExit.Invoke();
		
	}
	
	//------------------------------------------------------------------------------------------------
	void SetVehiclePriceText(int price)
	{
		m_wVehiclePriceText.SetText("$ " + EL_Utils.IntToMoneyFormat(price));
	}
	
    //------------------------------------------------------------------------------------------------
    override void OnMenuOpen()
    {
        m_wRoot = GetRootWidget();
		
		GetGame().GetInputManager().AddActionListener("MenuSelect", EActionTrigger.PRESSED, OnBuyVehicle);
		GetGame().GetInputManager().AddActionListener("MenuLeft", EActionTrigger.DOWN, OnMenuLeft);
		GetGame().GetInputManager().AddActionListener("MenuRight", EActionTrigger.DOWN, OnMenuRight);
		GetGame().GetInputManager().AddActionListener("MenuTabLeft", EActionTrigger.DOWN, OnApplyColor);
		
		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, InvokeOnMenuClose);
		
		m_RedSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("RedSlider"));
		m_GreenSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("GreenSlider"));
		m_BlueSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("BlueSlider"));
		
		m_wHandlingSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("HandlingSlider"));
		m_wTopSpeedSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("TopSpeedSlider"));
		m_wAccelerationSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("AccelerationSlider"));
		m_wBreakingSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("BreakingSlider"));
		m_wInventorySizeSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("InventorySizeSlider"));
		
		m_RedIndex = TextWidget.Cast(m_wRoot.FindAnyWidget("RedIndex"));
		m_GreenIndex = TextWidget.Cast(m_wRoot.FindAnyWidget("GreenIndex"));
		m_BlueIndex = TextWidget.Cast(m_wRoot.FindAnyWidget("BlueIndex"));
		
		m_wVehiclePriceText = TextWidget.Cast(m_wRoot.FindAnyWidget("VehiclePriceText"));
		m_wVehicleTitleText = TextWidget.Cast(m_wRoot.FindAnyWidget("VehicleTitle"));

    }
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuClose() 
	{
		GetGame().GetInputManager().RemoveActionListener("MenuSelect", EActionTrigger.PRESSED, OnBuyVehicle);
		GetGame().GetInputManager().RemoveActionListener("MenuLeft", EActionTrigger.DOWN, OnMenuLeft);
		GetGame().GetInputManager().RemoveActionListener("MenuRight", EActionTrigger.DOWN, OnMenuRight);
		GetGame().GetInputManager().RemoveActionListener("MenuBack", EActionTrigger.DOWN, OnMenuClose);
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		if (m_RedSlider && m_GreenSlider && m_BlueSlider)
		{
			int newR = m_RedSlider.GetCurrent();
			int newG = m_GreenSlider.GetCurrent();
			int newB = m_BlueSlider.GetCurrent();
			
			if (r == newR && g == newG && b == newB)
				return;
			
			r = newR;
			g = newG;
			b = newB;
			
			if (m_RedIndex && m_GreenIndex && m_BlueIndex)
			{
				m_RedIndex.SetText(newR.ToString());
				m_GreenIndex.SetText(newR.ToString());
				m_BlueIndex.SetText(newR.ToString());
			}
			
			Color nextColor = Color.FromRGBA(newR, newG, newB, 255);
			m_OnColorChange.Invoke(nextColor);
			
		}
		super.OnMenuUpdate(tDelta);
	}

}