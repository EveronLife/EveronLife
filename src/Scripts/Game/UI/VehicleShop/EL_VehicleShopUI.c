modded enum ChimeraMenuPreset {
    EL_VehicleShop,
	EL_Garage
}

class EL_VehicleShopUI: ChimeraMenuBase
{
    protected Widget m_wRoot;
	SliderWidget m_RedSlider, m_GreenSlider, m_BlueSlider, m_wHandlingSlider, m_wEngineSlider, m_wBrakingSlider, m_wInventorySizeSlider;
	TextWidget m_RedIndex, m_GreenIndex, m_BlueIndex, m_wVehiclePriceText, m_wVehicleTitleText, m_wInventorySizeAmount;
	protected ButtonWidget m_wBuyButton;
	protected int m_iRed, m_iGreen, m_iBlue;
	protected ref Color m_NewColor;

	ref ScriptInvoker<int> m_OnVehicleSelectionChanged = new ScriptInvoker();
	ref ScriptInvoker<ref Color> m_OnColorChange = new ScriptInvoker();
	ref ScriptInvoker<ref Color> m_OnBuyVehicle = new ScriptInvoker();
	ref ScriptInvoker m_OnExit = new ScriptInvoker();

	protected ResourceName m_lVehiclePreviewImage = "{E29CB33937B2122C}UI/Layouts/Editor/Toolbar/PlacingMenu/VehiclePreviewImg.layout";
	protected HorizontalLayoutWidget m_wVehiclePreviewList;

	protected int m_iCurPrice;
	protected bool m_bCanBuy;
	protected IEntity m_LocalPlayer;
	protected bool m_bGridFocused = true;

	//------------------------------------------------------------------------------------------------
	void PopulateVehicleImageGrid(array<ResourceName> vehicleImages)
	{
		int column;

		foreach (ResourceName vehicleImage : vehicleImages)
		{
			Widget newVehicleImage = GetGame().GetWorkspace().CreateWidgets(m_lVehiclePreviewImage, m_wVehiclePreviewList);


			//Set Icon
			ImageWidget imageWidget = ImageWidget.Cast(newVehicleImage.FindAnyWidget("VehicleImage"));
			imageWidget.LoadImageTexture(0, vehicleImage);

			//Add item to grid
			m_wVehiclePreviewList.AddChild(newVehicleImage);
			HorizontalLayoutSlot.SetPadding(newVehicleImage, 20, 0, 20, 0);

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

	//------------------------------------------------------------------------------------------------
	void MoveVehiclePreviewGrid(int offset)
	{
		FrameSlot.Move(m_wVehiclePreviewList, offset, 0);
	}

	//------------------------------------------------------------------------------------------------
	void EnableGridMovement()
	{
		m_bGridFocused = true;
	}

	//------------------------------------------------------------------------------------------------
	void DisableGridMovement()
	{
		m_bGridFocused = false;
	}
	//------------------------------------------------------------------------------------------------
	void OnMenuLeft()
	{
		if (m_bGridFocused)
			m_OnVehicleSelectionChanged.Invoke(-1);
	}

	//------------------------------------------------------------------------------------------------
	void OnMenuRight()
	{
		if (m_bGridFocused)
			m_OnVehicleSelectionChanged.Invoke(1);
	}

	//------------------------------------------------------------------------------------------------
	void OnBuyVehicle()
	{
		if (m_bCanBuy)
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
		m_iCurPrice = price;
		m_wVehiclePriceText.SetText("$ " + EL_FormatUtils.DecimalSeperator(price));
	}

	//------------------------------------------------------------------------------------------------
	//! Check if player can afford the vehicle and update button + text
	void ValidatePrice(int price)
	{
		m_bCanBuy = (EL_MoneyUtils.GetCash(m_LocalPlayer) >= price);
		if (m_bCanBuy)
		{
			m_wVehiclePriceText.SetColor(Color.DarkGreen);
			m_wBuyButton.SetEnabled(true);
			m_wBuyButton.SetOpacity(1);
		}
		else
		{
			m_wVehiclePriceText.SetColor(Color.DarkRed);
			m_wBuyButton.SetEnabled(false);
			m_wBuyButton.SetOpacity(0.5);
		}
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
		m_wInventorySizeAmount = TextWidget.Cast(m_wRoot.FindAnyWidget("StorageSizeText"));

		m_wVehiclePriceText = TextWidget.Cast(m_wRoot.FindAnyWidget("VehiclePriceText"));
		m_wVehicleTitleText = TextWidget.Cast(m_wRoot.FindAnyWidget("VehicleTitle"));

		//Preview Grid
		m_wVehiclePreviewList = HorizontalLayoutWidget.Cast(m_wRoot.FindAnyWidget("VehiclePreviewGrid"));

		//Buy Button
		m_wBuyButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("BuyButton"));
		SCR_NavigationButtonComponent buyButtonComp = SCR_NavigationButtonComponent.Cast(m_wBuyButton.FindHandler(SCR_NavigationButtonComponent));
		buyButtonComp.m_OnClicked.Insert(OnBuyVehicle);

		//Left Button
		ButtonWidget leftButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("LeftButton"));
		SCR_NavigationButtonComponent leftButtonComp = SCR_NavigationButtonComponent.Cast(leftButton.FindHandler(SCR_NavigationButtonComponent));
		leftButtonComp.m_OnClicked.Insert(OnMenuLeft);

		//Right Button
		ButtonWidget rightButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("RightButton"));
		SCR_NavigationButtonComponent rightButtonComp = SCR_NavigationButtonComponent.Cast(rightButton.FindHandler(SCR_NavigationButtonComponent));
		rightButtonComp.m_OnClicked.Insert(OnMenuRight);

		//Exit Button
		ButtonWidget exitButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("ExitButton"));
		SCR_NavigationButtonComponent exitButtonComp = SCR_NavigationButtonComponent.Cast(exitButton.FindHandler(SCR_NavigationButtonComponent));
		exitButtonComp.m_OnClicked.Insert(InvokeOnMenuClose);

		//Slider Focus
		SCR_EventHandlerComponent redSliderComp = SCR_EventHandlerComponent.Cast(m_RedSlider.FindHandler(SCR_EventHandlerComponent));
		SCR_EventHandlerComponent greenSliderComp = SCR_EventHandlerComponent.Cast(m_GreenSlider.FindHandler(SCR_EventHandlerComponent));
		SCR_EventHandlerComponent blueSliderComp = SCR_EventHandlerComponent.Cast(m_BlueSlider.FindHandler(SCR_EventHandlerComponent));
		redSliderComp.GetOnFocus().Insert(DisableGridMovement);
		greenSliderComp.GetOnFocus().Insert(DisableGridMovement);
		blueSliderComp.GetOnFocus().Insert(DisableGridMovement);
		redSliderComp.GetOnFocusLost().Insert(EnableGridMovement);
		greenSliderComp.GetOnFocusLost().Insert(EnableGridMovement);
		blueSliderComp.GetOnFocusLost().Insert(EnableGridMovement);

		//Get player using the UI
		m_LocalPlayer = SCR_PlayerController.GetLocalControlledEntity();
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

			if (m_iRed == newR && m_iGreen == newG && m_iBlue == newB)
				return;

			m_iRed = newR;
			m_iGreen = newG;
			m_iBlue = newB;

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