[EntityEditorProps(category: "EL/Garage")]
class EL_GarageManagerComponentClass : ScriptComponentClass
{
};

class EL_GarageManagerComponent : ScriptComponent
{
	// Point info?
	[Attribute("0.777 3.069 12.971", UIWidgets.EditBox, "Camera offset used for this PIP")]
	protected vector m_vCameraPoint;

	[Attribute("0 0 0", UIWidgets.EditBox, "Camera offset used for this PIP")]
	protected vector m_vCameraAngels;

	[Attribute("0 0 0", UIWidgets.EditBox, "Rotation Speed, 0 to disable")]
	protected vector m_vRotationSpeed;

	protected bool m_bIsEnabled;
	protected SCR_ManualCamera m_GarageCamera;

	protected ref map<string, ref array<string>> m_mSavedVehicles = new ref map<string, ref array<string>>;

	InputManager m_InputManager;
	IEntity m_GarageEntity;

	ResourceName m_GarageCameraPrefab = "{FAE60B62153B7058}Prefabs/Buildings/Garage/Garage_Camera.et";
	ResourceName m_EmptyVehiclePreview = "{6CA10EE93F1A3C20}Prefabs/Buildings/Garage/GaragePreviewVehicle.et";
	IEntity m_aPreviewVehicle;
	IEntity m_UserEntity;

	ref array<ref EL_GarageData> m_aGarageSaveDataList = new ref array<ref EL_GarageData>();
	int m_iCurPreviewVehicleIndex;
		
	EL_GarageUI m_GarageUI;

	//------------------------------------------------------------------------------------------------
	void DisableCam()
	{
		EnableGarageCamera(false);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_AskWithdrawVehicle()
	{
		IEntity freeSpawnPoint = EL_SpawnUtils.FindFreeSpawnPoint(SCR_EntityHelper.GetMainParent(GetOwner()));
		if (!freeSpawnPoint)
		{
			Print("[EL-Garage] No free spawn point to withdraw!", LogLevel.WARNING);
			return;
		}
		string ownerId = EL_Utils.GetPlayerUID(m_UserEntity);

		array<string> storedVehicleIds = m_mSavedVehicles.Get(ownerId);
		if (!storedVehicleIds || storedVehicleIds.IsEmpty())
			return;

		string withdrawnVehicleId = storedVehicleIds.Get(m_iCurPreviewVehicleIndex);
		storedVehicleIds.Remove(m_iCurPreviewVehicleIndex);
		m_mSavedVehicles.Set(ownerId, storedVehicleIds);

		//Load and spawn parked vehicle
		IEntity withdrawnVehicle = EL_PersistentWorldEntityLoader.Load(EL_VehicleSaveData, withdrawnVehicleId);

		//Teleport to free spawn point
		withdrawnVehicle.SetOrigin(freeSpawnPoint.GetOrigin());
		withdrawnVehicle.SetAngles(freeSpawnPoint.GetAngles());

		//Save data
		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(withdrawnVehicle.FindComponent(EL_PersistenceComponent));
		persistence.Save();

		DisableCam();
	}

	//------------------------------------------------------------------------------------------------
	void WithdrawVehicle()
	{
		Print("[EL-Garage] Asking Server to withdraw Vehicle");
		Rpc(Rpc_AskWithdrawVehicle);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnVehicleSelectionChanged(int offset)
	{
		int lastPreviewVehicleIndex = m_iCurPreviewVehicleIndex;
		m_iCurPreviewVehicleIndex += offset;
		
		if (lastPreviewVehicleIndex == m_iCurPreviewVehicleIndex)
			return;
		
		if (m_iCurPreviewVehicleIndex > (m_aGarageSaveDataList.Count() - 1))
			m_iCurPreviewVehicleIndex = 0;
		
		if (m_iCurPreviewVehicleIndex < 0)
			m_iCurPreviewVehicleIndex = m_aGarageSaveDataList.Count() - 1;

		SetVehiclePreviewMesh(m_iCurPreviewVehicleIndex);
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	private void OpenGarageUI()
	{
		m_GarageUI = EL_GarageUI.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.EL_Garage));

		//Listen to events
		m_GarageUI.m_OnVehicleSelectionChanged.Insert(OnVehicleSelectionChanged);
		m_GarageUI.m_OnTakeOutVehicle.Insert(WithdrawVehicle);
		m_GarageUI.m_OnExit.Insert(DisableCam);

		//Trigger first vehicle update
		OnVehicleSelectionChanged(0);

		//Load and populate preview images from config
		array<ResourceName> vehiclePreviewImages = {};
		foreach (EL_GarageData storedVehicleData : m_aGarageSaveDataList)
		{
			vehiclePreviewImages.Insert(EL_Utils.GetUIInfoPrefabIcon(storedVehicleData.m_rPrefab));
		}
		m_GarageUI.PopulateVehicleImageGrid(vehiclePreviewImages);
	}
	
	//------------------------------------------------------------------------------------------------
	void EnableGarageCamera(bool enabled)
	{
		if (enabled && !m_bIsEnabled)
		{
			// Create Garage camera
			if (!m_GarageCamera)
				m_GarageCamera = EL_CameraUtils.CreateAndSetCamera(m_GarageCameraPrefab, m_GarageEntity, m_vCameraPoint, m_vCameraAngels);

			m_bIsEnabled = true;
			return;
		}

		if (!enabled && m_bIsEnabled)
		{
			delete m_aPreviewVehicle;

			EL_CameraUtils.DestroyCamera(m_GarageCamera);
			m_bIsEnabled = false;
			return;
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Garage Stuff
	//------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------
	void SetVehiclePreviewMesh(int vehicleIndex)
	{
		if (m_aGarageSaveDataList.Count() == 0)
			return;

		EntitySpawnParams params();
		params.TransformMode = ETransformMode.WORLD;
		EL_SpawnUtils.FindSpawnPoint(m_GarageEntity).GetTransform(params.Transform);
		//Check if empty preview entity exists
		if (!m_aPreviewVehicle)
			m_aPreviewVehicle = GetGame().SpawnEntityPrefabLocal(Resource.Load(m_EmptyVehiclePreview), GetGame().GetWorld(), params);

		//Set new mesh
		VObject newVehicleMesh = EL_Utils.GetPrefabVObject(m_aGarageSaveDataList[vehicleIndex].m_rPrefab);

		m_aPreviewVehicle.SetObject(newVehicleMesh, "");
		//EL_Utils.SetColor(m_aPreviewVehicle, m_aGarageSaveDataList[vehicleIndex].m_iVehicleColor);

		Print("[EL-Garage] Setting new preview vehicle mesh: " + newVehicleMesh);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_SetVehicleSaveData(RplId playerId, array<ref EL_GarageData> garageSaveData)
	{
		Print("[EL-Garage] This client revieced 'RPC_SetVehicleSaveData' Broadcast");
		RplComponent rplC = RplComponent.Cast(Replication.FindItem(playerId));
		IEntity pUserEntity = rplC.GetEntity();
		IEntity localPlayer = SCR_PlayerController.GetLocalControlledEntity();

		if (pUserEntity != localPlayer)
			return;

		m_aGarageSaveDataList = garageSaveData;

		SetVehiclePreviewMesh(m_iCurPreviewVehicleIndex);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RPC_GetGarageSaveDataList(RplId playerId)
	{
		RplComponent rplC = RplComponent.Cast(Replication.FindItem(playerId));
		IEntity pUserEntity = rplC.GetEntity();
		array<string> allVehiclesInGarage = GetOwnedVehicles(EL_Utils.GetPlayerUID(pUserEntity));
		
		if (!allVehiclesInGarage)
			return;
		
		array<ref EL_GarageData> garageVehicleList = new array<ref EL_GarageData>();
		
		EL_DbRepository<EL_VehicleSaveData> vehicleRepo = EL_PersistenceEntityHelper<EL_VehicleSaveData>.GetRepository();
		
		foreach (string vehicleId : allVehiclesInGarage)
		{
			Print("[EL-Garage] Server loading vehicle from db: " + vehicleId);
			EL_VehicleSaveData vehSaveData = vehicleRepo.Find(vehicleId).GetEntity();
			
			EL_Utils.SetColor(GetOwner(), Color.FromInt(m_iVehicleColor));
			EL_Utils.SetSlotsColor(GetOwner(), m_iVehicleColor);
			
			array<ref EL_ComponentSaveDataBase> colorSaveData = vehSaveData.m_mComponentsSaveData.Get(EL_VehicleAppearanceSaveData);
			//EL_CarColorSaveData colorComp = EL_CarColorSaveData.Cast(colorSaveData.Get(0));

			EL_GarageData garageVehicleData = new EL_GarageData();
			garageVehicleData.m_rPrefab = vehSaveData.m_rPrefab;
			//garageVehicleData.m_iVehicleColor = colorComp.m_iVehicleColor;
			garageVehicleList.Insert(garageVehicleData);

		}
		//Host&Play check, cause broadcasts are dropped on server
		if (pUserEntity == SCR_PlayerController.GetLocalControlledEntity())
			RPC_SetVehicleSaveData(rplC.Id(), garageVehicleList);
		else
			Rpc(RPC_SetVehicleSaveData, rplC.Id(), garageVehicleList);
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	void PopulateLocalGarage()
	{
		m_iCurPreviewVehicleIndex = 0;

		Print("[EL-Garage] Asking server to spawn first preview Vehicle..");
		RplComponent rplC = RplComponent.Cast(m_UserEntity.FindComponent(RplComponent));
		Rpc(RPC_GetGarageSaveDataList, rplC.Id());
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	void OpenGarage(IEntity user)
	{
		m_UserEntity = user;
		PopulateLocalGarage();
		EnableGarageCamera(true);
		OpenGarageUI();
	}

	//------------------------------------------------------------------------------------------------
	void AddVehicle(string vehicleId, string ownerId)
	{
		Print("[EL-Garage] Added vehicle: " + vehicleId + " for " + ownerId);

		if (m_mSavedVehicles.Get(ownerId))
		{
			//Owner already has stuff in this garage
			ref array<string> storedVehicleIds = m_mSavedVehicles.Get(ownerId);
			storedVehicleIds.Insert(vehicleId);
			m_mSavedVehicles.Set(ownerId, storedVehicleIds);
		}
		else
		{
			//New owner for this garage
			m_mSavedVehicles.Insert(ownerId, {vehicleId});
		}
	}

	//------------------------------------------------------------------------------------------------
	void SetVehicles(map<string, ref array<string>> savedVehicles)
	{
		m_mSavedVehicles = savedVehicles;
	}

	//------------------------------------------------------------------------------------------------
	map<string, ref array<string>> GetAllVehicles()
	{
		return m_mSavedVehicles;
	}

	//------------------------------------------------------------------------------------------------
	//! Return null if no vehicles saved or no ownerID present
	array<string> GetOwnedVehicles(string ownerId)
	{
		array<string> storedVehicleIds = m_mSavedVehicles.Get(ownerId);
		if (storedVehicleIds)
			Print("Loading " + storedVehicleIds.Count() + " vehicles for " + ownerId);
		return storedVehicleIds;
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_aPreviewVehicle)
			m_aPreviewVehicle.SetAngles(m_aPreviewVehicle.GetAngles() + m_vRotationSpeed * timeSlice);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_InputManager = GetGame().GetInputManager();
		m_GarageEntity = owner.GetWorld().FindEntityByName("PREVIEW_GARAGE");
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, false);
		//Only set FRAME Mask if needed
		if (m_vRotationSpeed != vector.Zero)
		{
			SetEventMask(owner, EntityEvent.FRAME);
			owner.SetFlags(EntityFlags.ACTIVE, true);
		}
	}
};