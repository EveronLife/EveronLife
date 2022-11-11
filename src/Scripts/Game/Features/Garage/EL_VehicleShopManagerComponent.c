[EntityEditorProps(category: "EL/VehicleShop")]
class EL_VehicleShopManagerComponentClass : ScriptComponentClass
{
};

class EL_VehicleShopManagerComponent : ScriptComponent
{
	// Point info?
	[Attribute("0.777 3.069 12.971", UIWidgets.EditBox, "Camera offset used for this PIP")]
	protected vector m_vCameraPoint;

	[Attribute("0 0 0", UIWidgets.EditBox, "Camera offset used for this PIP")]
	protected vector m_vCameraAngels;

	[Attribute("0 0 0", UIWidgets.EditBox, "Rotation Speed, 0 to disable")]
	protected vector m_vRotationSpeed;

	[Attribute("", UIWidgets.Auto, "Item price list")]
	protected ref EL_PriceConfig m_PriceConfig;

	protected bool m_bIsEnabled;
	protected SCR_ManualCamera m_VehicleShopCamera;

	InputManager m_InputManager;
	IEntity m_GarageEntity;

	ResourceName m_VehicleShopCameraPrefab = "{FAE60B62153B7058}Prefabs/Buildings/Garage/Garage_Camera.et";
	ResourceName m_EmptyVehiclePreview = "{6CA10EE93F1A3C20}Prefabs/Buildings/Garage/GaragePreviewVehicle.et";
	ResourceName m_MissingPreviewIcon = "{AC7E384FF9D8016A}Common/Textures/placeholder_BCR.edds";

	SCR_BasePreviewEntity m_aPreviewVehicle;
	IEntity m_UserEntity;

	int m_iCurPreviewVehicleIndex;
	EL_VehicleShopUI m_VehicleShopUI;
	Color m_PreviewVehicleColor;

	//------------------------------------------------------------------------------------------------
	void OnExitMenu()
	{
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.EL_VehicleShop);
		DisableCam();
	}

	//------------------------------------------------------------------------------------------------
	BaseContainer GetBaseContainer(ResourceName prefab, string typeName)
	{
		IEntitySource prefabSource = Resource.Load(prefab).GetResource().ToEntitySource();
		int count = prefabSource.GetComponentCount();

		for(int i = 0; i < count; i++)
		{
			IEntityComponentSource comp = prefabSource.GetComponent(i);

			if(comp.GetClassName() == typeName)
			{
				return comp;
			}
		}
		return null;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetVehiclePrefabIcon(ResourceName prefab)
	{
		BaseContainer vehicleUIInfoComponent = GetBaseContainer(prefab, "SCR_EditableVehicleComponent");

		SCR_EditableVehicleUIInfo vehicleUIInfo;
		vehicleUIInfoComponent.Get("m_UIInfo", vehicleUIInfo);
		if (vehicleUIInfo)
			return vehicleUIInfo.GetImage();
		return m_MissingPreviewIcon;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetVehiclePrefabName(ResourceName prefab)
	{
		BaseContainer vehicleUIInfoComponent = GetBaseContainer(prefab, "SCR_EditableVehicleComponent");

		SCR_EditableVehicleUIInfo vehicleUIInfo;
		vehicleUIInfoComponent.Get("m_UIInfo", vehicleUIInfo);
		if (vehicleUIInfo)
			return vehicleUIInfo.GetName();
		return m_MissingPreviewIcon;
	}

	//------------------------------------------------------------------------------------------------
	float GetVehicleStorage(ResourceName prefab)
	{
		BaseContainer inventoryContainer = GetBaseContainer(prefab, "SCR_UniversalInventoryStorageComponent");

		float maxInvWeight;
		if (inventoryContainer)
			inventoryContainer.Get("m_fMaxWeight", maxInvWeight);

		return maxInvWeight;
	}

	//------------------------------------------------------------------------------------------------
	array<float> GetVehiclePrefabSimulation(ResourceName prefab)
	{
		BaseContainer vehicleSimulationContainer = GetBaseContainer(prefab, "VehicleWheeledSimulation");
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
		EL_Price curVehicleConfig = m_PriceConfig.m_aPriceConfigs[m_iCurPreviewVehicleIndex];

		if (m_aPreviewVehicle)
			SCR_EntityHelper.DeleteEntityAndChildren(m_aPreviewVehicle);

		//Spawn preview Vehicle with all slots (windows etc..)
		m_aPreviewVehicle = EL_LocalPrefabPreviewEntity.SpawnLocalPreviewFromPrefab(
			Resource.Load(curVehicleConfig.m_Prefab),
			"{150FDD1A8FC1E074}Prefabs/Buildings/Garage/BasePreviewEnt.et",
			EL_SpawnUtils.FindSpawnPoint(m_GarageEntity).GetOrigin()
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
		m_VehicleShopUI.m_wVehicleTitleText.SetText(GetVehiclePrefabName(curVehicleConfig.m_Prefab));

		//Update vehicle stats
		m_VehicleShopUI.m_wHandlingSlider.SetCurrent(vehicleStats[1]);
		m_VehicleShopUI.m_wEngineSlider.SetCurrent(vehicleStats[0]);
		m_VehicleShopUI.m_wBrakingSlider.SetCurrent(vehicleStats[2]);
		m_VehicleShopUI.m_wInventorySizeSlider.SetCurrent(maxInvWeight);

	}

	//------------------------------------------------------------------------------------------------
	void OnVehicleSelectionChanged(int offset)
	{
		int nextIndex = m_iCurPreviewVehicleIndex + offset;

		if (nextIndex > (m_PriceConfig.m_aPriceConfigs.Count() - 1) || nextIndex < 0)
			return;

		m_iCurPreviewVehicleIndex = nextIndex;

		m_VehicleShopUI.MoveVehiclePreviewGrid(offset * -304);

		UpdateVehicleStats();
	}

	//------------------------------------------------------------------------------------------------
	void ChangeColorRecursive(IEntity parent, Color color)
	{
		IEntity child = parent.GetChildren();
		while (child)
		{
			if (child.GetChildren())
				ChangeColorRecursive(child, color);

			EL_Utils.SetColor(child, color);
			child = child.GetSibling();
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	void OnColorChange(Color color)
	{
		m_PreviewVehicleColor = color;
		if (m_aPreviewVehicle)
		{
			ChangeColorRecursive(m_aPreviewVehicle, color);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	private void OpenVehicleShopUI()
	{
		m_VehicleShopUI = EL_VehicleShopUI.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.EL_VehicleShop));
		Print("OpenShopUI for " + EL_Utils.GetPlayerUID(SCR_PlayerController.GetLocalControlledEntity()));

		//Listen to events
		m_VehicleShopUI.m_OnVehicleSelectionChanged.Insert(OnVehicleSelectionChanged);
		m_VehicleShopUI.m_OnColorChange.Insert(OnColorChange);
		m_VehicleShopUI.m_OnBuyVehicle.Insert(BuyVehicle);
		m_VehicleShopUI.m_OnExit.Insert(OnExitMenu);

		//Trigger first vehicle update
		OnVehicleSelectionChanged(0);

		//Load and populate preview images
		array<ResourceName> vehiclePreviewImages = {};
		foreach (EL_Price price: m_PriceConfig.m_aPriceConfigs)
		{
			vehiclePreviewImages.Insert(GetVehiclePrefabIcon(price.m_Prefab));
		}
		m_VehicleShopUI.PopulateVehicleImageGrid(vehiclePreviewImages);

	}

	//------------------------------------------------------------------------------------------------
	void DisableCam()
	{
		EnableVehicleShopCamera(false);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_AskBuyVehicle(ResourceName vehiclePrefab, int color, string playerUID)
	{
		//Find free spawn point
		IEntity freeSpawnPoint = EL_SpawnUtils.FindFreeSpawnPoint(SCR_EntityHelper.GetMainParent(GetOwner()));
		if (!freeSpawnPoint)
		{
			Print("[EL-VehicleShop] No free spawn point to buy!", LogLevel.WARNING);
			return;
		}

		//Spawn new vehicle
		IEntity newVehicle = EL_Utils.SpawnEntityPrefab(vehiclePrefab, freeSpawnPoint.GetOrigin(), freeSpawnPoint.GetYawPitchRoll());

		//Set vehicle color and texture
		EL_VehicleAppearanceComponent vehicleAppearance = EL_VehicleAppearanceComponent.Cast(newVehicle.FindComponent(EL_VehicleAppearanceComponent));
		vehicleAppearance.SetVehicleColor(color);
		//vehicleAppearance.SetVehicleTexture("");

		//Set vehicle owner
		EL_CharacterOwnerComponent charOwnerComp = EL_CharacterOwnerComponent.Cast(newVehicle.FindComponent(EL_CharacterOwnerComponent));
		charOwnerComp.SetCharacterOwner(playerUID);

		//Save vehicle
		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(newVehicle.FindComponent(EL_PersistenceComponent));
		persistence.Save();
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	void BuyVehicle(Color color)
	{
		string playerUID = EL_Utils.GetPlayerUID(SCR_PlayerController.GetLocalControlledEntity());
		EL_Price curVehicleConfig = m_PriceConfig.m_aPriceConfigs[m_iCurPreviewVehicleIndex];

		//Ask server do buy vehicle
		Rpc(Rpc_AskBuyVehicle, curVehicleConfig.m_Prefab, color.PackToInt(), playerUID);

		//Cleanup
		DisableCam();
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.EL_VehicleShop);
	}

	//------------------------------------------------------------------------------------------------
	void EnableVehicleShopCamera(bool enabled)
	{
		if (enabled && !m_bIsEnabled)
		{
			// Create VehicleShop camera
			if (!m_VehicleShopCamera)
				m_VehicleShopCamera = EL_CameraUtils.CreateAndSetCamera(m_VehicleShopCameraPrefab, m_GarageEntity, m_vCameraPoint, m_vCameraAngels);

			m_bIsEnabled = true;
			return;
		}

		if (!enabled && m_bIsEnabled)
		{
			EL_CameraUtils.DestroyCamera(m_VehicleShopCamera);
			m_bIsEnabled = false;
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
		m_InputManager = GetGame().GetInputManager();
		m_GarageEntity = owner.GetWorld().FindEntityByName("VEHICLE_SHOP_PREVIEW");
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, false);
	}
};
