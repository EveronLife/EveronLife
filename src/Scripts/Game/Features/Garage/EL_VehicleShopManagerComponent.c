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

	IEntity m_aPreviewVehicle;
	IEntity m_UserEntity;

	int m_iCurPreviewVehicleIndex;
	EL_VehicleShopUI m_VehicleShopUI;

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

		//Get all the prefab data
		float maxInvWeight = GetVehicleStorage(curVehicleConfig.m_Prefab);
		array<float> vehicleStats = GetVehiclePrefabSimulation(curVehicleConfig.m_Prefab);

		//Update vehicle color
		OnColorChange(m_VehicleShopUI.GetCurrentSliderColor());

		//Update price
		m_VehicleShopUI.SetVehiclePriceText(curVehicleConfig.m_iBuyPrice);
		//Udate title
		m_VehicleShopUI.m_wVehicleTitleText.SetText(curVehicleConfig.m_sName);

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
		SetVehiclePreviewMesh(m_iCurPreviewVehicleIndex);
		UpdateVehicleStats();
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	void OnColorChange(Color color)
	{
		if (m_aPreviewVehicle)
		{
			EL_Utils.SetColor(m_aPreviewVehicle, color);
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

		//Populate Preview Images
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
	void Rpc_AskBuyVehicle()
	{
		IEntity freeSpawnPoint = EL_SpawnUtils.FindFreeSpawnPoint(SCR_EntityHelper.GetMainParent(GetOwner()));
		if (!freeSpawnPoint)
		{
			Print("[EL-VehicleShop] No free spawn point to withdraw!", LogLevel.WARNING);
			return;
		}
		string ownerId = EL_Utils.GetPlayerUID(m_UserEntity);


		Print("[EL-VehicleShop] Totally bought this vehicle!");
		//EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(withdrawnVehicle.FindComponent(EL_PersistenceComponent));
		//persistence.Save();
	}

	//------------------------------------------------------------------------------------------------
	void BuyVehicle()
	{
		Print("[EL-VehicleShop] Asking Server to buy Vehicle");
		Rpc(Rpc_AskBuyVehicle);
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
	void SetVehiclePreviewMesh(int vehicleIndex)
	{
		if (m_PriceConfig.m_aPriceConfigs.Count() == 0)
			return;

		EntitySpawnParams params();
		params.TransformMode = ETransformMode.WORLD;
		EL_SpawnUtils.FindSpawnPoint(m_GarageEntity).GetTransform(params.Transform);
		//Check if empty preview entity exists
		if (!m_aPreviewVehicle)
			m_aPreviewVehicle = GetGame().SpawnEntityPrefabLocal(Resource.Load(m_EmptyVehiclePreview), GetGame().GetWorld(), params);

		//Set new mesh
		VObject newVehicleMesh = EL_Utils.GetPrefabVObject(m_PriceConfig.m_aPriceConfigs[vehicleIndex].m_Prefab);

		m_aPreviewVehicle.SetObject(newVehicleMesh, "");
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
