[EntityEditorProps(category: "EL/VehicleShop")]
class EL_VehicleShopManagerComponentClass : ScriptComponentClass
{
}

class EL_VehicleShopManagerComponent : ScriptComponent
{
	
	[Attribute("VEHICLE_SHOP_PREVIEW", UIWidgets.Auto, "Item price list", category: "Preview")]
	protected string m_sShopPreviewBuildingName;
	protected IEntity m_VehicleShopBuilding;

	[Attribute("{6CA10EE93F1A3C20}Prefabs/Buildings/VehicleShop/VehicleShopPreviewVehicle.et", UIWidgets.ResourceNamePicker, "Camera prefab", category: "Preview")]
	protected ResourceName m_EmptyVehiclePreview;
	protected SCR_BasePreviewEntity m_aPreviewVehicle;

	[Attribute("{FAE60B62153B7058}Prefabs/Buildings/VehicleShop/VehicleShop_Camera.et", UIWidgets.ResourceNamePicker, "Camera prefab", category: "Preview")]
	protected ResourceName m_VehicleShopCameraPrefab;
	protected bool m_bCameraEnabled;
	protected SCR_ManualCamera m_VehicleShopCamera;

	// Point info
	[Attribute("0.777 3.069 12.971", UIWidgets.EditBox, "Camera point", category: "Preview")]
	protected vector m_vCameraPoint;

	[Attribute("0 0 0", UIWidgets.EditBox, "Camera angels", category: "Preview")]
	protected vector m_vCameraAngels;

	[Attribute("", UIWidgets.Auto, "Item price list", category: "Shop")]
	protected ref EL_VehiclePriceConfig m_VehiclePriceConfig;

	protected InputManager m_InputManager;
	protected IEntity m_UserEntity;

	protected int m_iCurPreviewVehicleIndex;
	protected EL_VehicleShopUI m_VehicleShopUI;
	protected Color m_PreviewVehicleColor;

	//------------------------------------------------------------------------------------------------
	void OnExitMenu()
	{
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.EL_VehicleShop);
		DisableCam();
	}

	//------------------------------------------------------------------------------------------------
	float GetVehicleStorage(ResourceName prefab)
	{
		BaseContainer inventoryContainer = SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), "SCR_UniversalInventoryStorageComponent");

		float maxInvWeight;
		if (inventoryContainer)
			inventoryContainer.Get("m_fMaxWeight", maxInvWeight);

		return maxInvWeight;
	}

	//------------------------------------------------------------------------------------------------
	array<float> GetVehiclePrefabSimulation(ResourceName prefab)
	{
		BaseContainer vehicleSimulationContainer = SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), "VehicleWheeledSimulation");
		BaseContainer vehicleSimulation = vehicleSimulationContainer.GetObject("Simulation");
		BaseContainer vehicleEngine = vehicleSimulation.GetObject("Engine");
		BaseContainer vehicleAxle = vehicleSimulation.GetObjectArray("Axles")[0];
		BaseContainer vehicleAxleTyre = vehicleAxle.GetObject("Tyre");

		//Engine
		float engineMaxPower;
		vehicleEngine.Get("MaxPower", engineMaxPower);
		//Handling
		float handlingPower;
		vehicleEngine.Get("Friction", handlingPower);
		//Braking
		float brakingPower;
		vehicleAxleTyre.Get("LongitudinalFriction", brakingPower);

		array<float> vehicleStats = {engineMaxPower, handlingPower, brakingPower};

		return vehicleStats;
	}

	//------------------------------------------------------------------------------------------------
	void UpdateVehicleStats()
	{
		EL_VehiclePrice curVehicleConfig = m_VehiclePriceConfig.m_aVehiclePriceConfigs[m_iCurPreviewVehicleIndex];


		if (m_aPreviewVehicle)
			SCR_EntityHelper.DeleteEntityAndChildren(m_aPreviewVehicle);


		//Spawn preview vehicle with all slots (windows etc..)
		m_aPreviewVehicle = EL_LocalPrefabPreviewEntity.SpawnLocalPreviewFromPrefab(
			Resource.Load(curVehicleConfig.m_Prefab),
			"{150FDD1A8FC1E074}Prefabs/Buildings/Garage/BasePreviewEnt.et",
			EL_SpawnUtils.FindSpawnPoint(m_VehicleShopBuilding).GetOrigin()
		);

		if (!m_aPreviewVehicle)
		{
			Print("Error spawning preview vehicle!", LogLevel.ERROR);
			return;
		}

		//Get all the prefab data
		float maxInvWeight = GetVehicleStorage(curVehicleConfig.m_Prefab);
		array<float> vehicleStats = GetVehiclePrefabSimulation(curVehicleConfig.m_Prefab);

		//Update vehicle color
		OnColorChange(m_VehicleShopUI.GetCurrentSliderColor());

		//Update price
		m_VehicleShopUI.SetVehiclePriceText(curVehicleConfig.m_iBuyPrice);

		//Update title
		SCR_EditableVehicleUIInfo prefabUIInfo = EL_UIInfoUtils.GetVehicleInfo(curVehicleConfig.m_Prefab);
		m_VehicleShopUI.m_wVehicleTitleText.SetText(prefabUIInfo.GetName());

		//Update vehicle stats
		m_VehicleShopUI.m_wHandlingSlider.SetCurrent(vehicleStats[1]);
		m_VehicleShopUI.m_wEngineSlider.SetCurrent(vehicleStats[0]);
		m_VehicleShopUI.m_wBrakingSlider.SetCurrent(vehicleStats[2]);
		m_VehicleShopUI.m_wInventorySizeSlider.SetCurrent(maxInvWeight);
		m_VehicleShopUI.m_wInventorySizeAmount.SetText(string.Format("Storage Size: %1kg", maxInvWeight));

		//Validate Price
		m_VehicleShopUI.ValidatePrice(curVehicleConfig.m_iBuyPrice);
	}

	//------------------------------------------------------------------------------------------------
	void OnVehicleSelectionChanged(int offset)
	{
		int nextIndex = m_iCurPreviewVehicleIndex + offset;

		if (nextIndex > (m_VehiclePriceConfig.m_aVehiclePriceConfigs.Count() - 1) || nextIndex < 0)
			return;

		m_iCurPreviewVehicleIndex = nextIndex;

		m_VehicleShopUI.MoveVehiclePreviewGrid(offset * -305);

		UpdateVehicleStats();
	}


	//------------------------------------------------------------------------------------------------
	//! Called from client
	void OnColorChange(Color color)
	{
		m_PreviewVehicleColor = color;
		if (m_aPreviewVehicle)
		{
			EL_Utils.ChangeColorRecursive(m_aPreviewVehicle, color);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	private void OpenVehicleShopUI()
	{
		m_VehicleShopUI = EL_VehicleShopUI.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.EL_VehicleShop));

		//Listen to events
		m_VehicleShopUI.m_OnVehicleSelectionChanged.Insert(OnVehicleSelectionChanged);
		m_VehicleShopUI.m_OnColorChange.Insert(OnColorChange);
		m_VehicleShopUI.m_OnBuyVehicle.Insert(BuyVehicle);
		m_VehicleShopUI.m_OnExit.Insert(OnExitMenu);

		//Trigger first vehicle update
		OnVehicleSelectionChanged(0);

		//Load and populate preview images from config
		array<ResourceName> vehiclePreviewImages = {};
		foreach (EL_VehiclePrice price: m_VehiclePriceConfig.m_aVehiclePriceConfigs)
		{
			SCR_EditableVehicleUIInfo prefabUIInfo = EL_UIInfoUtils.GetVehicleInfo(price.m_Prefab);
			vehiclePreviewImages.Insert(prefabUIInfo.GetImage());
		}
		m_VehicleShopUI.PopulateVehicleImageGrid(vehiclePreviewImages);
	}

	//------------------------------------------------------------------------------------------------
	//! Called from authority
	void DoBuyVehicle(ResourceName vehiclePrefab, int color, IEntity player)
	{
		//Find free spawn point
		IEntity freeSpawnPoint = EL_SpawnUtils.FindFreeSpawnPoint(SCR_EntityHelper.GetMainParent(GetOwner()));
		if (!freeSpawnPoint)
		{
			SCR_HintManagerComponent.ShowCustomHint("All spawn points occupied", "Vehicle Shop", 5);
			return;
		}

		//Remove money
		foreach (EL_VehiclePrice price: m_VehiclePriceConfig.m_aVehiclePriceConfigs)
		{
			if (vehiclePrefab == price.m_Prefab)
				EL_MoneyUtils.RemoveCash(player, price.m_iBuyPrice);
		}
		
		//Spawn new vehicle
		IEntity newVehicle = EL_Utils.SpawnEntityPrefab(vehiclePrefab, freeSpawnPoint.GetOrigin(), freeSpawnPoint.GetYawPitchRoll());

		//Delete inventory items
		SCR_VehicleInventoryStorageManagerComponent vehicleStorage = EL_ComponentFinder<SCR_VehicleInventoryStorageManagerComponent>.Find(newVehicle);
		array<IEntity> allItems = {};
		vehicleStorage.GetItems(allItems);
		foreach (IEntity item : allItems)
		{
			vehicleStorage.TryDeleteItem(item);
		}

		//Set vehicle base color and texture
		EL_VehicleAppearanceComponent vehicleAppearance = EL_VehicleAppearanceComponent.Cast(newVehicle.FindComponent(EL_VehicleAppearanceComponent));
		vehicleAppearance.SetVehicleColor(color);

		//Set slots color and texture
		EL_Utils.SetSlotsColor(newVehicle, color);

		//Set vehicle owner
		EL_CharacterOwnerComponent charOwnerComp = EL_CharacterOwnerComponent.Cast(newVehicle.FindComponent(EL_CharacterOwnerComponent));
		charOwnerComp.SetCharacterOwner(EL_Utils.GetPlayerUID(player));

		//Save vehicle
		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(newVehicle.FindComponent(EL_PersistenceComponent));
		persistence.Save();
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	void BuyVehicle(Color color)
	{
		EL_VehiclePrice curVehicleConfig = m_VehiclePriceConfig.m_aVehiclePriceConfigs[m_iCurPreviewVehicleIndex];

		EL_RpcSenderComponent rpcSender = EL_RpcSenderComponent.Cast(m_UserEntity.FindComponent(EL_RpcSenderComponent));
		rpcSender.AskBuyVehicle(curVehicleConfig.m_Prefab, color.PackToInt(), GetOwner());

		//Cleanup
		DisableCam();
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.EL_VehicleShop);
	}

	//------------------------------------------------------------------------------------------------
	void DisableCam()
	{
		EnableVehicleShopCamera(false);
	}

	//------------------------------------------------------------------------------------------------
	void EnableVehicleShopCamera(bool enabled)
	{
		if (enabled && !m_bCameraEnabled)
		{
			// Create VehicleShop camera
			if (!m_VehicleShopCamera)
				m_VehicleShopCamera = EL_CameraUtils.CreateAndSetCamera(m_VehicleShopCameraPrefab, m_VehicleShopBuilding, m_vCameraPoint, m_vCameraAngels);

			m_bCameraEnabled = true;
			return;
		}

		if (!enabled && m_bCameraEnabled)
		{
			EL_CameraUtils.DestroyCamera(m_VehicleShopCamera);
			m_bCameraEnabled = false;
			return;
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	void OpenVehicleShop(IEntity user)
	{
		m_UserEntity = user;
		m_iCurPreviewVehicleIndex = 0;

		EnableVehicleShopCamera(true);
		OpenVehicleShopUI();
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		if (!GetGame().InPlayMode())
			return;
		m_InputManager = GetGame().GetInputManager();
		m_VehicleShopBuilding = owner.GetWorld().FindEntityByName(m_sShopPreviewBuildingName);
		if (!m_VehicleShopBuilding)
			Print("Shop Preview building not found!", LogLevel.ERROR);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, false);
	}
}