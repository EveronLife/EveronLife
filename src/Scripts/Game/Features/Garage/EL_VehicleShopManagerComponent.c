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
	ResourceName m_lVehiclePreviewImage = "{E29CB33937B2122C}UI/Layouts/Editor/Toolbar/PlacingMenu/VehiclePreviewImg.layout";
	
	IEntity m_aPreviewVehicle;
	IEntity m_UserEntity;

	int m_iCurPreviewVehicleIndex;
	EL_VehicleShopUI m_VehicleShopUI;

	void OnExitMenu()
	{
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.EL_VehicleShop);
		DisableCam();
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateVehicleStats()
	{		
		Widget vehicleImage = GetGame().GetWorkspace().CreateWidgets(m_lVehiclePreviewImage);
		
		m_VehicleShopUI.SetVehiclePriceText(m_PriceConfig.m_aPriceConfigs[m_iCurPreviewVehicleIndex].m_iBuyPrice);
		OnColorChange(m_VehicleShopUI.GetCurrentSliderColor());
		m_VehicleShopUI.m_wVehicleTitleText.SetText(m_PriceConfig.m_aPriceConfigs[m_iCurPreviewVehicleIndex].m_sName);
		
		
		IEntity tempVehicle = GetGame().SpawnEntityPrefabLocal(Resource.Load(m_PriceConfig.m_aPriceConfigs[m_iCurPreviewVehicleIndex].m_Prefab));
		RplComponent rplComp = RplComponent.Cast(tempVehicle.FindComponent(RplComponent));
		rplComp.Deactivate(tempVehicle);
		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(tempVehicle.FindComponent(EL_PersistenceComponent));
		persistence.Detach();
		
		VehicleWheeledSimulation vehSimulation = VehicleWheeledSimulation.Cast(tempVehicle.FindComponent(VehicleWheeledSimulation));
		float friction = vehSimulation.WheelTyreGetLongitudinalFriction(0);
		m_VehicleShopUI.m_wHandlingSlider.SetCurrent(friction);	
		m_VehicleShopUI.m_wTopSpeedSlider.SetCurrent(vehSimulation.EngineGetRPMPeakPower());
		m_VehicleShopUI.m_wAccelerationSlider.SetCurrent(vehSimulation.EngineGetPeakPower());
		
		SCR_UniversalInventoryStorageComponent vehInventoryComp = SCR_UniversalInventoryStorageComponent.Cast(tempVehicle.FindComponent(SCR_UniversalInventoryStorageComponent));
		if (vehInventoryComp)
			m_VehicleShopUI.m_wInventorySizeSlider.SetCurrent(vehInventoryComp.GetMaxLoad());
		
		
		SlotManagerComponent slotMan = SlotManagerComponent.Cast(tempVehicle.FindComponent(SlotManagerComponent));
		if (slotMan)
		{
			array<EntitySlotInfo> slots = {};
			slotMan.GetSlotInfos(slots);
			EntitySlotInfo entSlot = slots[0];
			Print(entSlot.GetAttachedEntity());
		}
		
		SCR_EntityHelper.DeleteEntityAndChildren(tempVehicle);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnVehicleSelectionChanged(int offset)
	{
		int lastPreviewVehicleIndex = m_iCurPreviewVehicleIndex;
		m_iCurPreviewVehicleIndex += offset;

		if (m_iCurPreviewVehicleIndex > (m_PriceConfig.m_aPriceConfigs.Count() - 1))
			m_iCurPreviewVehicleIndex = 0;
		
		if (m_iCurPreviewVehicleIndex < 0)
			m_iCurPreviewVehicleIndex = m_PriceConfig.m_aPriceConfigs.Count() - 1;

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
		
		m_VehicleShopUI.m_OnVehicleSelectionChanged.Insert(OnVehicleSelectionChanged);
		m_VehicleShopUI.m_OnColorChange.Insert(OnColorChange);
		m_VehicleShopUI.m_OnBuyVehicle.Insert(BuyVehicle);
		m_VehicleShopUI.m_OnExit.Insert(OnExitMenu);
		
		OnVehicleSelectionChanged(0);
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
	//! VehicleShop Stuff
	//------------------------------------------------------------------------------------------------

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


modded class SCR_UniversalInventoryStorageComponent
{
	float GetMaxLoad() { return m_fMaxWeight; }
}
	