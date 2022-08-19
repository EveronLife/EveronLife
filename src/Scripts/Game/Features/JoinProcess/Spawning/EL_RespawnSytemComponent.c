[ComponentEditorProps(category: "EveronLife/Core/Spawning", description: "Player spawn manager")]
class EL_RespawnSytemComponentClass : SCR_RespawnSystemComponentClass
{
}

class EL_RespawnSytemComponent : SCR_RespawnSystemComponent
{
	[Attribute(defvalue: "{37578B1666981FCE}Prefabs/Characters/Core/Character_Base.et", desc: "Character prefab", category: "New character defaults")]
	protected ResourceName m_rDefaultCharacterPrefab;
	
	// TODO: Options to configure new character spawn loadout
	
	protected ref map<int, ref EL_CharacterSaveData> m_mSpawnData = new map<int, ref EL_CharacterSaveData>();
	
	//------------------------------------------------------------------------------------------------
	void SetSpawnData(int playerId, EL_CharacterSaveData saveData)
	{
		m_mSpawnData.Set(playerId, saveData);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override GenericEntity RequestSpawn(int playerId)
	{
		GenericEntity playerEntity;
		
		vector position;
		vector angles;
		
		EL_SpawnPoint spawnPoint = EL_SpawnPoint.GetDefaultSpawnPoint();
		if(!spawnPoint)
		{
			Print("Could not spawn character, no default spawn point configured.", LogLevel.ERROR);
			return null;
		}
		
		spawnPoint.GetPosAngles(position, angles);
		
		EL_CharacterSaveData saveData = m_mSpawnData.Get(playerId);
		if(saveData && saveData.m_rPrefab)
		{
			//PrintFormat("Spawning existing character with id '%1'.", saveData.GetId());

			EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
			
			playerEntity = GenericEntity.Cast(persistenceManager.SpawnWorldEntity(saveData));
			
			// Validate and return if persistence component is active, aka save data loaded and entity ready to be used.
			EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(playerEntity.FindComponent(EL_PersistenceComponent));
			if(persistenceComponent && persistenceComponent.IsActive()) return playerEntity;
			
			Debug.Error(string.Format("Failed to apply save-data '%1:%2' to character.", saveData.Type(), saveData.GetId()));
			SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
		}
		
		// Spawn default character
		//Print("Creating new character.");
		
		if(!m_rDefaultCharacterPrefab)
		{
			Print("Could not create new character, no default prefab configured. Go to EL_GameModeRoleplay > EL_RespawnSytemComponent and choose one.", LogLevel.ERROR);
			return null;
		}
		
		playerEntity = DoSpawn(m_rDefaultCharacterPrefab, position, angles);
		
		// Add new character to account
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(playerEntity.FindComponent(EL_PersistenceComponent));
		if(!persistenceComponent)
		{
			Print(string.Format("Could not create new character, prefab '%1' is missing component '%2'.", m_rDefaultCharacterPrefab, EL_PersistenceComponent), LogLevel.ERROR);
			SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
			return null;
		}
		
		// Add new character to account
		EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetAccount(EL_Utils.GetPlayerUID(playerId));
		account.m_aCharacterIds.Insert(persistenceComponent.GetPersistentId());
		
		return playerEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity owner)
	{
		// Hard override to not rely on faction or loadout manager
		m_pGameMode = EL_GameModeRoleplay.Cast(owner);
		
		if (m_pGameMode)
		{
			// Use replication of the parent
			m_pRplComponent = RplComponent.Cast(m_pGameMode.FindComponent(RplComponent));
		}
		else
		{
			Print("EL_RespawnSytemComponent has to be attached to a EL_GameModeRoleplay (or inherited) entity!", LogLevel.ERROR);
		}
	}
}
