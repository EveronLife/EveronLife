class EL_GarageUI: ChimeraMenuBase
{
    protected Widget m_wRoot;
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
		EL_GarageEntry garageEntry = EL_GarageEntry.Cast(button.GetData());
		m_GarageManager.WithdrawVehicle(garageEntry.m_iIndex);
		Close();
	}

	//------------------------------------------------------------------------------------------------
	void PopulateVehicleList(array<ResourceName> garagePrefabList)
	{
		foreach (int index, ResourceName vehiclePrefab : garagePrefabList)
		{
			Widget vehicleListEntry = GetGame().GetWorkspace().CreateWidgets(m_VehiclePreviewImage, m_wVehiclePreviewList);
			SCR_EditableVehicleUIInfo prefabUIInfo = EL_UIInfoUtils.GetVehicleInfo(vehiclePrefab);

			ImageWidget imageWidget = ImageWidget.Cast(vehicleListEntry.FindAnyWidget("VehicleImage"));
			imageWidget.LoadImageTexture(0, prefabUIInfo.GetImage());

			TextWidget nameText = TextWidget.Cast(vehicleListEntry.FindAnyWidget("VehicleTitle"));
			nameText.SetText(prefabUIInfo.GetName());

			ButtonWidget vehicleWithdrawButton = ButtonWidget.Cast(vehicleListEntry);
			SCR_ModularButtonComponent entryButton = SCR_ModularButtonComponent.Cast(vehicleWithdrawButton.FindHandler(SCR_ModularButtonComponent));

			EL_GarageEntry garageData = new EL_GarageEntry();
			garageData.m_sPrefab = vehiclePrefab;
			garageData.m_iIndex = index;

			entryButton.SetData(garageData);
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
