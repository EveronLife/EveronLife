[ComponentEditorProps(category: "EveronLife/Feature/Character/Spawning", description: "Part of the re-spawn system on the gamemode.")]
class EL_RespawnHandlerComponentClass : SCR_RespawnHandlerComponentClass
{
};

class EL_RespawnHandlerComponent : SCR_RespawnHandlerComponent
{
	protected EL_RespawnSytemComponent m_pRespawnSystem;
	protected PlayerManager m_pPlayerManager;

	//------------------------------------------------------------------------------------------------
	override void OnPlayerRegistered(int playerId)
	{
		if (!m_pGameMode.IsMaster())
			return;

		string playerUid = EL_Utils.GetPlayerUID(playerId);
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
			// New account, skip to new character spawn
			m_pRespawnSystem.PrepareCharacter(playerInfo.param1, null);
			m_sEnqueuedPlayers.Insert(playerInfo.param1);
			return;
		}

		// Load first available character until selection flow is implemented
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

		// Prepare spawn data buffer with last known player data (null for fresh accounts) and queue player for spawn
		m_pRespawnSystem.PrepareCharacter(characterInfo.param1, characterData);
		m_sEnqueuedPlayers.Insert(characterInfo.param1);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		m_sEnqueuedPlayers.RemoveItem(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		if (!m_pGameMode.IsMaster())
			return;

		// Add the dead body root entity collection so it spawns back after restart for looting
		EL_PersistenceComponent persistence = EL_Component<EL_PersistenceComponent>.Find(player);
		persistence.ForceSelfSpawn();

		// Delete the dead char from account
		EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetFromCache(player);
		if (account)
			account.m_aCharacterIds.RemoveItem(persistence.GetPersistentId());

		// Prepare and execute fresh character spawn
		m_pRespawnSystem.PrepareCharacter(playerId, null);
		m_sEnqueuedPlayers.Insert(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		if (!m_pGameMode.IsMaster())
			return;

		m_sEnqueuedPlayers.RemoveItem(playerId);

		IEntity player = m_pPlayerManager.GetPlayerController(playerId).GetControlledEntity();
		if (player)
		{
			EL_PersistenceComponent persistence = EL_Component<EL_PersistenceComponent>.Find(player);
			persistence.PauseTracking();
			persistence.Save();
		}

		EL_PlayerAccountManager.GetInstance().SaveAndReleaseAccount(EL_Utils.GetPlayerUID(playerId));
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_sEnqueuedPlayers.IsEmpty())
			return;

		set<int> iterCopy();
		iterCopy.Copy(m_sEnqueuedPlayers);
		foreach (int playerId : iterCopy)
		{
			if (m_pRespawnSystem.IsReadyForSpawn(playerId))
				m_pPlayerManager.GetPlayerController(playerId).RequestRespawn();
		}
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_pRespawnSystem = EL_Component<EL_RespawnSytemComponent>.Find(owner);
		m_pPlayerManager = GetGame().GetPlayerManager();
	}
};
