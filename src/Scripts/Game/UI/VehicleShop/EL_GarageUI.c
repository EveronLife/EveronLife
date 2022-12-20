class EL_GarageUI: ChimeraMenuBase
{
    protected Widget m_wRoot;
	protected TextWidget m_wVehicleTitleText;
	protected ButtonWidget m_wTakeOutButton;

	protected ResourceName m_VehiclePreviewImage = "{28E9E144675337A3}UI/Layouts/Garage/GarageVehiclePreviewImg.layout";
	protected VerticalLayoutWidget m_wVehiclePreviewList;

	protected EL_GarageManagerComponent m_GarageManager;

	//------------------------------------------------------------------------------------------------
	void SetGarageManager(EL_GarageManagerComponent garageManager)
	{
		m_GarageManager = garageManager;
	}

	//------------------------------------------------------------------------------------------------
	protected void OnVehicleEntryClicked(SCR_ModularButtonComponent button)
	{
		EL_GarageData vehicleData = EL_GarageData.Cast(button.GetData());
		m_GarageManager.WithdrawVehicle(vehicleData.m_iIndex);
		Close();
	}

	//------------------------------------------------------------------------------------------------
	void PopulateVehicleList(array<ref EL_GarageData> garageSaveDataList)
	{
		foreach (int index, EL_GarageData vehicleData : garageSaveDataList)
		{
			vehicleData.m_iIndex = index;
			Widget vehicleListEntry = GetGame().GetWorkspace().CreateWidgets(m_VehiclePreviewImage, m_wVehiclePreviewList);
		
			ImageWidget imageWidget = ImageWidget.Cast(vehicleListEntry.FindAnyWidget("VehicleImage"));
			imageWidget.LoadImageTexture(0, EL_Utils.GetUIInfoPrefabIcon(vehicleData.m_rPrefab));
			
			TextWidget nameText = TextWidget.Cast(vehicleListEntry.FindAnyWidget("VehicleTitle"));
			nameText.SetText(EL_Utils.GetUIInfoName(vehicleData.m_rPrefab));
	
			ButtonWidget vehicleWithdrawButton = ButtonWidget.Cast(vehicleListEntry);
			SCR_ModularButtonComponent entryButton = SCR_ModularButtonComponent.Cast(vehicleWithdrawButton.FindHandler(SCR_ModularButtonComponent));
			entryButton.SetData(vehicleData);
			entryButton.m_OnClicked.Insert(OnVehicleEntryClicked);
			
			m_wVehiclePreviewList.AddChild(vehicleListEntry);			
		}
	}

    //------------------------------------------------------------------------------------------------
    override void OnMenuOpen()
    {
        m_wRoot = GetRootWidget();
		m_wVehiclePreviewList = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("GarageVehicleList"));
		ButtonWidget exitButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("ExitButton"));
		SCR_ModularButtonComponent exitButtonComp = SCR_ModularButtonComponent.Cast(exitButton.FindHandler(SCR_ModularButtonComponent));
		exitButtonComp.m_OnClicked.Insert(Close);
    }
}
