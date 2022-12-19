[EntityEditorProps(category: "EL/Garage")]
class EL_GarageManagerComponentClass : ScriptComponentClass
{
};

class EL_GarageManagerComponent : ScriptComponent
{
	protected ref map<string, ref array<string>> m_mSavedVehicles = new ref map<string, ref array<string>>;
	IEntity m_UserEntity;

	ref array<ref EL_GarageData> m_aGarageSaveDataList = new ref array<ref EL_GarageData>();

	EL_GarageUI m_GarageUI;

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_AskWithdrawVehicle(int index)
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

		string withdrawnVehicleId = storedVehicleIds.Get(index);
		storedVehicleIds.Remove(index);
		m_mSavedVehicles.Set(ownerId, storedVehicleIds);

		//Load and spawn parked vehicle
		IEntity withdrawnVehicle = EL_PersistentWorldEntityLoader.Load(EL_VehicleSaveData, withdrawnVehicleId);

		//Teleport to free spawn point
		withdrawnVehicle.SetOrigin(freeSpawnPoint.GetOrigin());
		withdrawnVehicle.SetAngles(freeSpawnPoint.GetAngles());

		//Save data
		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(withdrawnVehicle.FindComponent(EL_PersistenceComponent));
		persistence.Save();
	}

	//------------------------------------------------------------------------------------------------
	void WithdrawVehicle(int index)
	{
		Rpc(Rpc_AskWithdrawVehicle, index);
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	private void OpenGarageUI()
	{
		m_GarageUI = EL_GarageUI.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.EL_Garage));
		m_GarageUI.SetGarageManager(this);
		m_GarageUI.PopulateVehicleList(m_aGarageSaveDataList);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_SetVehicleSaveData(RplId targetPlayerId, array<ref EL_GarageData> garageSaveData)
	{
		Print("[EL-Garage] This client revieced 'RPC_SetVehicleSaveData' Broadcast");
		RplComponent targetRplComponent = RplComponent.Cast(Replication.FindItem(targetPlayerId));
		IEntity targetPlayer = targetRplComponent.GetEntity();
		IEntity localPlayer = SCR_PlayerController.GetLocalControlledEntity();

		//Broadcast not for this player
		if (targetPlayer != localPlayer)
			return;

		m_aGarageSaveDataList = garageSaveData;
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

			//EL_Utils.SetColor(GetOwner(), Color.FromInt(m_iVehicleColor));
			//EL_Utils.SetSlotsColor(GetOwner(), m_iVehicleColor);

			//array<ref EL_ComponentSaveDataBase> colorSaveData = vehSaveData.m_mComponentsSaveData.Get(EL_VehicleAppearanceSaveData);
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
	void OpenGarage(IEntity user)
	{
		m_UserEntity = user;
		RplComponent rplC = RplComponent.Cast(m_UserEntity.FindComponent(RplComponent));
		//Get Garage Vehicles from Server
		Rpc(RPC_GetGarageSaveDataList, rplC.Id());

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
	override void EOnInit(IEntity owner)
	{

	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, false);
	}
};