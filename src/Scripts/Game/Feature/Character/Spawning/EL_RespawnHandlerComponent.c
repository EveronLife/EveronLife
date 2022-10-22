[ComponentEditorProps(category: "EveronLife/Feature/Character/Spawning", description: "Part of the re-spawn system on the gamemode.")]
class EL_RespawnHandlerComponentClass: SCR_RespawnHandlerComponentClass
{
}

class EL_RespawnHandlerComponent : SCR_RespawnHandlerComponent
{
	protected EL_RespawnSytemComponent m_pRespawnSystem;

	//------------------------------------------------------------------------------------------------
	override void OnPlayerConnected(int playerId)
	{
		if (!m_pGameMode.IsMaster()) return;

		// Hard override to ignore all the base component logic we do not need
		m_sEnqueuedPlayers.Insert(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		if (!m_pGameMode.IsMaster()) return;

		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(player.FindComponent(EL_PersistenceComponent));

		// Add the dead body root entity collection so it spawns back after restart for looting
		EL_PersistenceManagerInternal.GetInternalInstance().GetRootEntityCollection().Add(persistence, false, true);

		// Delete the dead char from account
		EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetFromCache(player);
		if (account) account.m_aCharacterIds.RemoveItem(persistence.GetPersistentId());

		// Prepare and execute fresh character spawn
		m_pRespawnSystem.SetSpawnData(playerId, null);
		m_sEnqueuedPlayers.Insert(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerDisconnected(int playerId)
	{
		if (!m_pGameMode.IsMaster()) return;

		int idx = m_sEnqueuedPlayers.Find(playerId);
		if (idx != -1) m_sEnqueuedPlayers.Remove(idx);

		m_pRespawnSystem.RemoveSpawnData(playerId);

		PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerId);
		if (playerController)
		{
			IEntity player = playerController.GetControlledEntity();
			if (player)
			{
				EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(player.FindComponent(EL_PersistenceComponent));
				persistence.Save();
				persistence.Detach();
			}
		}

		EL_PlayerAccountManager.GetInstance().SaveAndReleaseAccount(EL_Utils.GetPlayerUID(playerId));
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_pGameMode.IsMaster() || (EL_PersistenceManager.GetInstance().GetState() < EL_EPersistenceManagerState.ACTIVE)) return;

		// Wait until the player uid is available for the queued join players
		array<int> removeIds;
		foreach (int playerId : m_sEnqueuedPlayers)
		{
			string playerUid = EL_Utils.GetPlayerUID(playerId);
			if (playerUid)
			{
				LoadPlayerData(playerId, playerUid);
				if (!removeIds) removeIds = new array<int>();
				removeIds.Insert(playerId)
			}
		}

		// Remove any ids from queue that the loading via uid was kicked off for
		if (!removeIds) return;
		foreach (int removeId : removeIds)
		{
			int idx = m_sEnqueuedPlayers.Find(removeId);
			if (idx != -1) m_sEnqueuedPlayers.Remove(idx);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Starts async loading of the players data
	protected void LoadPlayerData(int playerId, string playerUid)
	{
		if (m_pRespawnSystem.HasSpawnData(playerId))
		{
			// Player data already known from previous event e.g. respawn, skip loading ...
			GetGame().GetPlayerManager().GetPlayerController(playerId).RequestRespawn();
			return;
		}

		EL_PlayerAccountCallback callback(new Tuple2<int, string>(playerId, playerUid));
		callback.ConfigureInvoker(this, "OnAccountLoaded");
		EL_PlayerAccountManager.GetInstance().LoadAccountAsync(playerUid, true, callback);
	}

	//------------------------------------------------------------------------------------------------
	//! Handles the account information found for the player
	protected void OnAccountLoaded(Managed context, EL_PlayerAccount account)
	{
		Tuple2<int, string> playerInfo = Tuple2<int, string>.Cast(context);

		if (account.m_aCharacterIds.IsEmpty())
		{
			m_pRespawnSystem.SetSpawnData(playerInfo.param1, null);
			GetGame().GetPlayerManager().GetPlayerController(playerInfo.param1).RequestRespawn();
			return;
		}

		string characterId = account.m_aCharacterIds.Get(0);
		Tuple3<int, string, string> characterContext(playerInfo.param1, playerInfo.param2, characterId);
		EL_DbFindCallbackSingle<EL_CharacterSaveData> characterDataCallback(characterContext);
		characterDataCallback.ConfigureInvoker(this, "OnCharacterDataLoaded");
		EL_PersistenceEntityHelper<EL_CharacterSaveData>.GetRepository().FindAsync(characterId, characterDataCallback);
	}

	//------------------------------------------------------------------------------------------------
	//! Handles the character data found for the players account
	protected void OnCharacterDataLoaded(Managed context, EL_EDbOperationStatusCode statusCode, EL_CharacterSaveData characterData)
	{
		/*
			TODO: Use respawn component system for pre spawn communication with player so they can choose the char to spawn with and where potentially
				- Component is on the player controller and is allowed to send rpcs to server (s. SCR_RespawnComponent for example setup)
				- Spawn on last location (if previously spawned)
				- Select spawns on map (depending on which job is active to enable things such as police spawns)
					- Maybe list with addtional spawns per job + city spawns every person has.
				- different position would just need to override the saveData transformation component data.
		*/

		Tuple3<int, string, string> characterInfo = Tuple3<int, string, string>.Cast(context);

		if (!characterData)
		{
			Print(string.Format("Failed to load existing character '%1' from account '%2'.", characterInfo.param3, characterInfo.param2), LogLevel.ERROR);
		}

		// Prepare spawn data buffer with last known player data (null for fresh accounts) and spawn character
		m_pRespawnSystem.SetSpawnData(characterInfo.param1, characterData);
		GetGame().GetPlayerManager().GetPlayerController(characterInfo.param1).RequestRespawn();
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		m_pRespawnSystem = EL_RespawnSytemComponent.Cast(owner.FindComponent(EL_RespawnSytemComponent));
	}
}
