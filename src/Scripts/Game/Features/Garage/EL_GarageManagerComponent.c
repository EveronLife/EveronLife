[EntityEditorProps(category: "EL/Garage")]
class EL_GarageManagerComponentClass : ScriptComponentClass
{
}

class EL_GarageManagerComponent : ScriptComponent
{
	protected ref map<string, ref array<string>> m_mSavedVehicles = new map<string, ref array<string>>;
	IEntity m_UserEntity;
	EL_GarageUI m_GarageUI;

	//------------------------------------------------------------------------------------------------
	void DoWithdrawVehicle(IEntity owner, int index)
	{
		IEntity freeSpawnPoint = EL_SpawnUtils.FindFreeSpawnPoint(SCR_EntityHelper.GetMainParent(GetOwner()));
		if (!freeSpawnPoint)
		{
			Print("[EL-Garage] No free spawn point to withdraw!", LogLevel.WARNING);
			return;
		}
		string ownerId = EL_Utils.GetPlayerUID(owner);

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
		EL_RpcSenderComponent rpcSender = EL_RpcSenderComponent.Cast(m_UserEntity.FindComponent(EL_RpcSenderComponent));
		rpcSender.AskWithdrawVehicle(GetOwner(), index);
	}

	//------------------------------------------------------------------------------------------------
	//! Called from client
	void OpenGarageUI(array<ResourceName> garageSaveDataList)
	{
		m_GarageUI = EL_GarageUI.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.EL_Garage));
		m_GarageUI.SetGarageManager(this);

		if (garageSaveDataList.Count() == 0)
			return;

		m_GarageUI.PopulateVehicleList(garageSaveDataList);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RPC_SetVehicleSaveData(RplId targetPlayerId, array<ResourceName> garageSaveData)
	{
		RplComponent targetRplComponent = RplComponent.Cast(Replication.FindItem(targetPlayerId));
		IEntity targetPlayer = targetRplComponent.GetEntity();
		IEntity localPlayer = SCR_PlayerController.GetLocalControlledEntity();

		//Broadcast not for this player
		if (targetPlayer != localPlayer)
			return;

		OpenGarageUI(garageSaveData);
	}

	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	void DoLoadGarage(IEntity pUserEntity)
	{
		array<string> allVehiclesInGarage = GetOwnedVehicles(EL_Utils.GetPlayerUID(pUserEntity));

		array<ResourceName> garageVehicleList = new array<ResourceName>();
		EL_DbRepository<EL_VehicleSaveData> vehicleRepo = EL_PersistenceEntityHelper<EL_VehicleSaveData>.GetRepository();

		if (allVehiclesInGarage)
		{
			foreach (string vehicleId : allVehiclesInGarage)
			{
				EL_VehicleSaveData vehSaveData = vehicleRepo.Find(vehicleId).GetEntity();
				garageVehicleList.Insert(vehSaveData.m_rPrefab);
			}
		}

		//Host&Play check
		RplComponent rplC = EL_ComponentFinder<RplComponent>.Find(pUserEntity);
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

		EL_RpcSenderComponent rpcSender = EL_RpcSenderComponent.Cast(m_UserEntity.FindComponent(EL_RpcSenderComponent));
		rpcSender.AskLoadGarage(GetOwner());
	}

	//------------------------------------------------------------------------------------------------
	void AddVehicle(string vehicleId, string ownerId)
	{
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
	array<string> GetOwnedVehicles(string ownerId)
	{
		array<string> storedVehicleIds = m_mSavedVehicles.Get(ownerId);
		return storedVehicleIds;
	}
}
