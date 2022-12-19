class EL_GarageUI: ChimeraMenuBase
{
    Widget m_wRoot;
	TextWidget m_wVehicleTitleText;
	ButtonWidget m_wTakeOutButton;

	ref Color m_NewColor;
	ResourceName m_VehiclePreviewImage = "{28E9E144675337A3}UI/Layouts/Editor/Toolbar/PlacingMenu/GarageVehiclePreviewImg.layout";
	VerticalLayoutWidget m_wVehiclePreviewList;
	
	EL_GarageManagerComponent m_GarageManager;
	
	//------------------------------------------------------------------------------------------------
	void OnVehicleEntryClicked(SCR_ModularButtonComponent button)
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
			
			//Set Icon
			ImageWidget imageWidget = ImageWidget.Cast(vehicleListEntry.FindAnyWidget("VehicleImage"));
			imageWidget.LoadImageTexture(0, EL_Utils.GetUIInfoPrefabIcon(vehicleData.m_rPrefab));

			//Set Vehicle Name
			TextWidget nameText = TextWidget.Cast(vehicleListEntry.FindAnyWidget("VehicleTitle"));
			nameText.SetText(EL_Utils.GetUIInfoName(vehicleData.m_rPrefab));
			
			//Setup button
			ButtonWidget vehicleWithdrawButton = ButtonWidget.Cast(vehicleListEntry);
			SCR_ModularButtonComponent vehicleWithdrawButtonComponent = SCR_ModularButtonComponent.Cast(vehicleWithdrawButton.FindHandler(SCR_ModularButtonComponent));
			vehicleWithdrawButtonComponent.SetData(vehicleData);
			vehicleWithdrawButtonComponent.m_OnClicked.Insert(OnVehicleEntryClicked);
			
			//Add item to list
			m_wVehiclePreviewList.AddChild(vehicleListEntry);
		}
	}

    //------------------------------------------------------------------------------------------------
    override void OnMenuOpen()
    {
        m_wRoot = GetRootWidget();
		m_wVehicleTitleText = TextWidget.Cast(m_wRoot.FindAnyWidget("VehicleTitle"));
		m_wVehiclePreviewList = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("VehiclePreviewGrid"));
		m_wTakeOutButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("TakeOutButton"));
    }
}
