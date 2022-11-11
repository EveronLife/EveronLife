modded enum ChimeraMenuPreset {
    EL_VehicleShop
}

class EL_VehicleShopUI: ChimeraMenuBase
{
    Widget m_wRoot;
	SliderWidget m_RedSlider, m_GreenSlider, m_BlueSlider, m_wHandlingSlider, m_wEngineSlider, m_wBrakingSlider, m_wInventorySizeSlider;
	TextWidget m_RedIndex, m_GreenIndex, m_BlueIndex, m_wVehiclePriceText, m_wVehicleTitleText;
	int r,g,b;
	ref Color m_NewColor;
	
	ref ScriptInvoker<int> m_OnVehicleSelectionChanged = new ScriptInvoker();
	ref ScriptInvoker<ref Color> m_OnColorChange = new ScriptInvoker();
	ref ScriptInvoker<ref Color> m_OnBuyVehicle = new ScriptInvoker();
	ref ScriptInvoker m_OnExit = new ScriptInvoker();
	
	
	ResourceName m_lVehiclePreviewImage = "{E29CB33937B2122C}UI/Layouts/Editor/Toolbar/PlacingMenu/VehiclePreviewImg.layout";
	UniformGridLayoutWidget m_wVehiclePreviewGrid;
	
	//------------------------------------------------------------------------------------------------
	void PopulateVehicleImageGrid(array<ResourceName> vehicleImages)
	{
		int column;
		
		foreach (ResourceName vehicleImage : vehicleImages)
		{
			
			Widget newVehicleImage = GetGame().GetWorkspace().CreateWidgets(m_lVehiclePreviewImage, m_wVehiclePreviewGrid);
			
			//Set Postion in grid
			UniformGridSlot.SetColumn(newVehicleImage, column);

			//Set Icon
			ImageWidget imageWidget = ImageWidget.Cast(newVehicleImage.FindAnyWidget("VehicleImage"));
			imageWidget.LoadImageTexture(0, vehicleImage);
			
			//Add item to grid
			m_wVehiclePreviewGrid.AddChild(newVehicleImage);

			column++;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	Color GetCurrentSliderColor()
	{
		int newR = m_RedSlider.GetCurrent();
		int newG = m_GreenSlider.GetCurrent();
		int newB = m_BlueSlider.GetCurrent();
		return Color.FromSRGBA(newR,newG,newB,255);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnApplyColor()
	{
		m_OnColorChange.Invoke(GetCurrentSliderColor());
	}		
	
	void MoveVehiclePreviewGrid(int offset)
	{
		FrameSlot.Move(m_wVehiclePreviewGrid, offset, 0);
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
		m_OnBuyVehicle.Invoke(m_NewColor);
		
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
		
		//Color picker
		m_RedSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("RedSlider"));
		m_GreenSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("GreenSlider"));
		m_BlueSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("BlueSlider"));
		
		m_RedIndex = TextWidget.Cast(m_wRoot.FindAnyWidget("RedIndex"));
		m_GreenIndex = TextWidget.Cast(m_wRoot.FindAnyWidget("GreenIndex"));
		m_BlueIndex = TextWidget.Cast(m_wRoot.FindAnyWidget("BlueIndex"));
		
		//Vehicle Stats
		m_wEngineSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("EngineSlider"));
		m_wHandlingSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("HandlingSlider"));
		m_wBrakingSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("BrakingSlider"));
		m_wInventorySizeSlider = SliderWidget.Cast(m_wRoot.FindAnyWidget("InventorySizeSlider"));
				
		m_wVehiclePriceText = TextWidget.Cast(m_wRoot.FindAnyWidget("VehiclePriceText"));
		m_wVehicleTitleText = TextWidget.Cast(m_wRoot.FindAnyWidget("VehicleTitle"));
		
		//Preview Grid
		m_wVehiclePreviewGrid = UniformGridLayoutWidget.Cast(m_wRoot.FindAnyWidget("VehiclePreviewGrid"));
		
		
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
				m_GreenIndex.SetText(newG.ToString());
				m_BlueIndex.SetText(newB.ToString());
			}
			
			m_NewColor = Color.FromSRGBA(newR, newG, newB, 255);
			m_OnColorChange.Invoke(m_NewColor);
			
		}
		super.OnMenuUpdate(tDelta);
	}

}