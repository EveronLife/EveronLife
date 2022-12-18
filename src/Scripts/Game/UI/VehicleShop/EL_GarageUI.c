class EL_GarageUI: ChimeraMenuBase
{
    Widget m_wRoot;
	TextWidget m_wVehicleTitleText;
	ButtonWidget m_wTakeOutButton;

	ref Color m_NewColor;

	ref ScriptInvoker<int> m_OnVehicleSelectionChanged = new ScriptInvoker();
	
	ref ScriptInvoker m_OnTakeOutVehicle = new ScriptInvoker();
	ref ScriptInvoker m_OnExit = new ScriptInvoker();

	ResourceName m_VehiclePreviewImage = "{E29CB33937B2122C}UI/Layouts/Editor/Toolbar/PlacingMenu/VehiclePreviewImg.layout";
	UniformGridLayoutWidget m_wVehiclePreviewGrid;

	private IEntity m_LocalPlayer;
	
	//------------------------------------------------------------------------------------------------
	void PopulateVehicleImageGrid(array<ResourceName> vehicleImages)
	{
		int column;

		foreach (ResourceName vehicleImage : vehicleImages)
		{

			Widget newVehicleImage = GetGame().GetWorkspace().CreateWidgets(m_VehiclePreviewImage, m_wVehiclePreviewGrid);

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
	void OnTakeOutVehicle()
	{
		m_OnTakeOutVehicle.Invoke(m_NewColor);
	}
	
	//------------------------------------------------------------------------------------------------
	void InvokeOnMenuClose()
	{
		m_OnExit.Invoke();

	}

    //------------------------------------------------------------------------------------------------
    override void OnMenuOpen()
    {
        m_wRoot = GetRootWidget();

		GetGame().GetInputManager().AddActionListener("MenuSelect", EActionTrigger.PRESSED, OnTakeOutVehicle);
		GetGame().GetInputManager().AddActionListener("MenuLeft", EActionTrigger.DOWN, OnMenuLeft);
		GetGame().GetInputManager().AddActionListener("MenuRight", EActionTrigger.DOWN, OnMenuRight);
		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, InvokeOnMenuClose);
		
		m_wVehicleTitleText = TextWidget.Cast(m_wRoot.FindAnyWidget("VehicleTitle"));

		//Preview Grid
		m_wVehiclePreviewGrid = UniformGridLayoutWidget.Cast(m_wRoot.FindAnyWidget("VehiclePreviewGrid"));

		m_wTakeOutButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("TakeOutButton"));
		//Get player using the UI
		m_LocalPlayer = SCR_PlayerController.GetLocalControlledEntity();
    }

	//------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		GetGame().GetInputManager().RemoveActionListener("MenuSelect", EActionTrigger.PRESSED, OnTakeOutVehicle);
		GetGame().GetInputManager().RemoveActionListener("MenuLeft", EActionTrigger.DOWN, OnMenuLeft);
		GetGame().GetInputManager().RemoveActionListener("MenuRight", EActionTrigger.DOWN, OnMenuRight);
		GetGame().GetInputManager().RemoveActionListener("MenuBack", EActionTrigger.DOWN, OnMenuClose);
	}
}