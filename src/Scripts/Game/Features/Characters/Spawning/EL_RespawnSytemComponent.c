[ComponentEditorProps(category: "EveronLife/Core/Spawning", description: "Player spawn manager")]
class EL_RespawnSytemComponentClass : SCR_RespawnSystemComponentClass
{
}

class EL_RespawnSytemComponent : SCR_RespawnSystemComponent
{
	[Attribute(category: "New character defaults")]
	protected ref array<ResourceName> m_rDefaultCharacterPrefabs;
	
	[Attribute(category: "New character defaults")]
	protected ref array<ref EL_DefaultCharacterLoadoutSlot> m_aDefaultCharacterItems;
	
	protected ref map<int, ref EL_CharacterSaveData> m_mSpawnData = new map<int, ref EL_CharacterSaveData>();
	
	//------------------------------------------------------------------------------------------------
	void SetSpawnData(int playerId, EL_CharacterSaveData saveData)
	{
		m_mSpawnData.Set(playerId, saveData);
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasSpawnData(int playerId)
	{
		return m_mSpawnData.Contains(playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override GenericEntity RequestSpawn(int playerId)
	{
		GenericEntity playerEntity;
		
		EL_CharacterSaveData saveData = m_mSpawnData.Get(playerId);
		if(saveData && saveData.m_rPrefab)
		{
			EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
			
			persistenceManager.SetNextPersistentId(saveData.GetId());
			EL_TransformationSaveData tmData = EL_TransformationSaveData.Cast(saveData.m_mComponentsSaveData.Get(EL_TransformationSaveData).Get(0));
			saveData.m_mComponentsSaveData.Remove(EL_TransformationSaveData);
			playerEntity = DoSpawn(saveData.m_rPrefab, tmData.m_vOrigin, Vector(tmData.m_vAngles[1], tmData.m_vAngles[0], tmData.m_vAngles[2]));
						
			// Validate and return if persistence component is active, aka save data loaded and entity ready to be used.
			EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(playerEntity.FindComponent(EL_PersistenceComponent));
			if(!persistenceComponent || !persistenceComponent.Load(saveData))
			{
				Debug.Error(string.Format("Failed to apply save-data '%1:%2' to character.", saveData.Type(), saveData.GetId()));
				SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
				playerEntity = null;
			}
		}
		
		if(!playerEntity)
		{
			if(m_rDefaultCharacterPrefabs.IsEmpty())
			{
				Print("Could not create new character, no default prefabs configured. Go to EL_GameModeRoleplay > EL_RespawnSytemComponent and add at least one.", LogLevel.ERROR);
				return null;
			}
			
			vector position;
			vector angles;
			
			EL_SpawnPoint spawnPoint = EL_SpawnPoint.GetDefaultSpawnPoint();
			if(!spawnPoint)
			{
				Print("Could not spawn character, no default spawn point configured.", LogLevel.ERROR);
				return null;
			}
			
			spawnPoint.GetPosAngles(position, angles);
			
			ResourceName charPrefab = m_rDefaultCharacterPrefabs.GetRandomElement();
			
			playerEntity = DoSpawn(charPrefab, position, angles);
			
			InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(playerEntity.FindComponent(InventoryStorageManagerComponent));
			BaseLoadoutManagerComponent loadoutManager = BaseLoadoutManagerComponent.Cast(playerEntity.FindComponent(BaseLoadoutManagerComponent));
			foreach(EL_DefaultCharacterLoadoutSlot defaultLoadoutSlot : m_aDefaultCharacterItems)
			{
				if(defaultLoadoutSlot.m_eArea == ELoadoutArea.ELA_None || !defaultLoadoutSlot.m_pItem) continue;
				
				IEntity slotEntity = SpawnDefaultCharacterItem(storageManager, defaultLoadoutSlot.m_pItem);
				if(!slotEntity) continue;
				
				loadoutManager.Wear(slotEntity);
			}
			
			// Add new character to account
			EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(playerEntity.FindComponent(EL_PersistenceComponent));
			if(!persistenceComponent)
			{
				Print(string.Format("Could not create new character, prefab '%1' is missing component '%2'.", charPrefab, EL_PersistenceComponent), LogLevel.ERROR);
				SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
				return null;
			}
			
			// Add new character to account
			EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetAccount(EL_Utils.GetPlayerUID(playerId));
			account.m_aCharacterIds.Insert(persistenceComponent.GetPersistentId());
		}
		
		return playerEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	protected IEntity SpawnDefaultCharacterItem(InventoryStorageManagerComponent storageManager, EL_DefaultCharacterLoadoutItem loadoutItem)
	{
		IEntity slotEntity = GetGame().SpawnEntityPrefab(Resource.Load(loadoutItem.m_rPrefab));
		if(!slotEntity) return null;
		
		if(loadoutItem.m_aStoredItems)
		{
			array<Managed> outComponents();
			slotEntity.FindComponents(BaseInventoryStorageComponent, outComponents);

			foreach(EL_DefaultCharacterLoadoutItem storedItem : loadoutItem.m_aStoredItems)
			{
				IEntity spawnedItem = SpawnDefaultCharacterItem(storageManager, storedItem);
				
				foreach(Managed componentRef : outComponents)
				{
					BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(componentRef);
					if(storageComponent.GetPurpose() & storedItem.m_ePurpose)
					{
						if(!storageManager.TryInsertItemInStorage(spawnedItem, storageComponent)) continue;
						
						InventoryItemComponent inventoryItemComponent = InventoryItemComponent.Cast(spawnedItem.FindComponent(InventoryItemComponent));
						if(inventoryItemComponent && !inventoryItemComponent.GetParentSlot()) continue;
						
						break;
					}
				}
			}
		}
		
		return slotEntity;
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

[BaseContainerProps(), EL_BaseContainerCustomTitleFieldEnum("m_eArea", ELoadoutArea)]
class EL_DefaultCharacterLoadoutSlot
{
	[Attribute(uiwidget: UIWidgets.ComboBox, desc: "Loadout area", enums: ParamEnumArray.FromEnum(ELoadoutArea))]
	ELoadoutArea m_eArea;
	
	[Attribute(desc: "Loadout item")]
	ref EL_DefaultCharacterLoadoutItem m_pItem;
}

[BaseContainerProps()]
class EL_DefaultCharacterLoadoutItem
{
	[Attribute()]
	ResourceName m_rPrefab;
	
	[Attribute(defvalue: EStoragePurpose.PURPOSE_DEPOSIT.ToString(), uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EStoragePurpose))]
	EStoragePurpose m_ePurpose;
	
	[Attribute()]
	ref array<ref EL_DefaultCharacterLoadoutItem> m_aStoredItems;
}
