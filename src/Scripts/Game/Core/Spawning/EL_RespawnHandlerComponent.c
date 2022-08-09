[ComponentEditorProps(category: "EveronLife/Core/Spawning", description: "Compatibility class, does nothing")]
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
		
		// Hard override to ignore base logic
		int idx = m_sEnqueuedPlayers.Find(playerId);
		if(idx != -1) m_sEnqueuedPlayers.Remove(idx);
		
		// TODO: delete current character from db, new one will get a new id.
		
		// Prepare and execute fresh character spawn
		m_pRespawnSystem.SetSpawnData(playerId, null);
		GetGame().GetPlayerManager().GetPlayerController(playerId).RequestRespawn();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerDisconnected(int playerId)
	{
		if (!m_pGameMode.IsMaster()) return;
		
		// Hard override to ignore base logic
		int idx = m_sEnqueuedPlayers.Find(playerId);
		if(idx != -1) m_sEnqueuedPlayers.Remove(idx);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_pGameMode.IsMaster() || !EL_PersistenceManager.GetInstance().IsActive()) return;
		
		// Wait until the player uid is available for the queued join players
		array<int> removeIds;
		foreach(int playerId : m_sEnqueuedPlayers)
		{
			string playerUid = EL_Utils.GetPlayerUID(playerId);
			if(playerUid)
			{
				LoadPlayerData(playerId, playerUid);
				if(!removeIds) removeIds = new array<int>();
				removeIds.Insert(playerId)
			}
		}
		
		// Remove any ids from queue that the loading via uid was kicked off for
		if(!removeIds) return;
		foreach(int removeId : removeIds)
		{
			int idx = m_sEnqueuedPlayers.Find(removeId);
			if(idx != -1) m_sEnqueuedPlayers.Remove(idx);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void LoadPlayerData(int playerId, string playerUid)
	{
		thread LoadPlayerDataThreadImpl(playerId, playerUid);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void LoadPlayerDataThreadImpl(int playerId, string playerUid)
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
		EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetAccount(playerUid);
		
		/*
			TODO: Use respawn component system for pre spawn communication with player so they can choose the char to spawn with and where potentially
				- Component is on the player controller and is allowed to send rpcs to server (s. SCR_RespawnComponent for example setup)
				- Spawn on last location (if previously spawned)
				- Select spawns on map (depending on which job is active to enable things such as police spawns)
					- Maybe list with addtional spawns per job + city spawns every person has.
				- different position would just need to override the saveData transformation component data.
		*/
		
		EL_CharacterSaveData characterData;
		if(account.m_aCharacterIds.Count() > 0)
		{
			EL_DbRepository<EL_CharacterSaveData> characterRepository = EL_DbEntityHelper<EL_CharacterSaveData>.GetRepository(persistenceManager.GetDbContext());
			characterData = characterRepository.Find(account.m_aCharacterIds.Get(0)).GetEntity();
			
			if(!characterData)
			{
				Print(string.Format("Failed to load existing character '%1' from account '%2'.", account.m_aCharacterIds.Get(0), playerUid), LogLevel.ERROR);
			}
		}
		
		// Prepare spawn data buffer with last known player data and spawn character
		m_pRespawnSystem.SetSpawnData(playerId, characterData);
		GetGame().GetPlayerManager().GetPlayerController(playerId).RequestRespawn();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_pRespawnSystem = EL_RespawnSytemComponent.Cast(owner.FindComponent(EL_RespawnSytemComponent));
	}
}
