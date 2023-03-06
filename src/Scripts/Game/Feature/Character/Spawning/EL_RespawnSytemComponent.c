[ComponentEditorProps(category: "EveronLife/Feature/Character/Spawning", description: "Part of the re-spawn system on the gamemode.")]
class EL_RespawnSytemComponentClass : SCR_RespawnSystemComponentClass
{
}

class EL_RespawnSytemComponent : SCR_RespawnSystemComponent
{
	[Attribute(category: "New character defaults")]
	protected ref array<ResourceName> m_aDefaultCharacterPrefabs;

	[Attribute(category: "New character defaults")]
	protected ref array<ref EL_DefaultLoadoutItem> m_aDefaultCharacterItems;

	protected ref map<int, ref EL_CharacterSaveData> m_mSpawnData = new map<int, ref EL_CharacterSaveData>();

	//------------------------------------------------------------------------------------------------
	void SetSpawnData(int playerId, EL_CharacterSaveData saveData)
	{
		m_mSpawnData.Set(playerId, saveData);
	}

	//------------------------------------------------------------------------------------------------
	void RemoveSpawnData(int playerId)
	{
		m_mSpawnData.Remove(playerId);
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
		if (saveData && saveData.m_rPrefab)
		{
			EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();

			persistenceManager.SetNextPersistentId(saveData.GetId());
			EL_TransformationSaveData tmData = EL_TransformationSaveData.Cast(saveData.m_mComponentsSaveData.Get(EL_TransformationSaveData).Get(0));
			saveData.m_mComponentsSaveData.Remove(EL_TransformationSaveData);
			playerEntity = DoSpawn(saveData.m_rPrefab, tmData.m_vOrigin, Vector(tmData.m_vAngles[1], tmData.m_vAngles[0], tmData.m_vAngles[2]));

			// Validate and return if persistence component is active, aka save-data loaded and entity ready to be used.
			EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(playerEntity.FindComponent(EL_PersistenceComponent));
			if (persistenceComponent && persistenceComponent.Load(saveData))
			{
				SCR_RespawnComponent respawnComponent = SCR_RespawnComponent.Cast(GetGame().GetPlayerManager().GetPlayerRespawnComponent(playerId));
				if (respawnComponent)
				{
					EL_RespawnCharacterState state();
					state.m_eStance = saveData.m_eStance;

					IEntity leftHandEntity = persistenceManager.FindEntityByPersistentId(saveData.m_sLeftHandItemId);
					if (leftHandEntity)
					{
						RplComponent replication = RplComponent.Cast(leftHandEntity.FindComponent(RplComponent));
						if (replication) state.m_pLeftHandItemRplId = replication.Id();
					}
					else
					{
						state.m_pLeftHandItemRplId = RplId.Invalid();
					}

					IEntity rightHandEntity = persistenceManager.FindEntityByPersistentId(saveData.m_sRightHandItemId);
					if (rightHandEntity)
					{
						RplComponent replication = RplComponent.Cast(rightHandEntity.FindComponent(RplComponent));
						if (replication)
						{
							state.m_pRightHandItemRplId = replication.Id();
							state.m_eRightHandType = saveData.m_eRightHandType;
							state.m_bRightHandRaised = saveData.m_bRightHandRaised;
						}
					}
					else
					{
						state.m_pRightHandItemRplId = RplId.Invalid();
					}

					state.m_aQuickBarRplIds = new array<RplId>();
					SCR_CharacterInventoryStorageComponent inventoryStorage = SCR_CharacterInventoryStorageComponent.Cast(playerEntity.FindComponent(SCR_CharacterInventoryStorageComponent));
					if (inventoryStorage)
					{
						// Init with invalid ids
						state.m_aQuickBarRplIds.Reserve(inventoryStorage.GetQuickSlotItems().Count());
						for (int i = 0, count = state.m_aQuickBarRplIds.Count(); i < count; i++)
						{
							state.m_aQuickBarRplIds.Insert(RplId.Invalid());
						}

						foreach (EL_PersistentQuickSlotItem quickSlot : saveData.m_aQuickSlotEntities)
						{
							IEntity slotEntity = persistenceManager.FindEntityByPersistentId(quickSlot.m_sEntityId);
							if (slotEntity && quickSlot.m_iIndex < state.m_aQuickBarRplIds.Count())
							{
								RplComponent replication = RplComponent.Cast(slotEntity.FindComponent(RplComponent));
								if (replication) state.m_aQuickBarRplIds.Set(quickSlot.m_iIndex, replication.Id());
							}
						}

						// Apply quick item slots serverside to avoid inital sync back from client with same data
						inventoryStorage.EL_Rpc_UpdateQuickSlotItems(state.m_aQuickBarRplIds);
					}

					respawnComponent.SetRespawnCharacterState(state);
				}
			}
			else
			{
				Debug.Error(string.Format("Failed to apply save-data '%1:%2' to character.", saveData.Type(), saveData.GetId()));
				SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
				playerEntity = null;
			}
		}

		if (!playerEntity)
		{
			if (m_aDefaultCharacterPrefabs.IsEmpty())
			{
				Print("Could not create new character, no default prefabs configured. Go to EL_GameModeRoleplay > EL_RespawnSytemComponent and add at least one.", LogLevel.ERROR);
				return null;
			}

			vector position;
			vector angles;

			EL_SpawnPoint spawnPoint = EL_SpawnPoint.GetDefaultSpawnPoint();
			if (!spawnPoint)
			{
				Print("Could not spawn character, no default spawn point configured.", LogLevel.ERROR);
				return null;
			}

			spawnPoint.GetPosAngles(position, angles);

			ResourceName charPrefab = m_aDefaultCharacterPrefabs.GetRandomElement();

			playerEntity = DoSpawn(charPrefab, position, angles);

			InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(playerEntity.FindComponent(InventoryStorageManagerComponent));
			BaseLoadoutManagerComponent loadoutManager = BaseLoadoutManagerComponent.Cast(playerEntity.FindComponent(BaseLoadoutManagerComponent));
			foreach (EL_DefaultLoadoutItem loadoutItem : m_aDefaultCharacterItems)
			{
				IEntity slotEntity = SpawnDefaultCharacterItem(storageManager, loadoutItem);
				if (!slotEntity) continue;
				loadoutManager.Wear(slotEntity);
			}

			// Add new character to account
			EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(playerEntity.FindComponent(EL_PersistenceComponent));
			if (!persistenceComponent)
			{
				Print(string.Format("Could not create new character, prefab '%1' is missing component '%2'.", charPrefab, EL_PersistenceComponent), LogLevel.ERROR);
				SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
				return null;
			}

			// Add new character to account
			EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetFromCache(EL_Utils.GetPlayerUID(playerId));
			if (account) account.m_aCharacterIds.Insert(persistenceComponent.GetPersistentId());
		}

		return playerEntity;
	}

	//------------------------------------------------------------------------------------------------
	//! Recursively spawn items configured in the spawn loadout to the character
	//! \param storageManager Storage manager of the character to give the items to
	//! \param loadoutItem Loadout configuration item
	//! \return parent entity of the items spawned by the configuration or null on failure
	protected IEntity SpawnDefaultCharacterItem(InventoryStorageManagerComponent storageManager, EL_DefaultLoadoutItem loadoutItem)
	{
		IEntity slotEntity = GetGame().SpawnEntityPrefab(Resource.Load(loadoutItem.m_rPrefab));
		if (!slotEntity) return null;

		if (loadoutItem.m_aComponentDefaults)
		{
			foreach (EL_DefaultLoadoutItemComponent componentDefault : loadoutItem.m_aComponentDefaults)
			{
				componentDefault.ApplyTo(slotEntity);
			}
		}
		
		if (loadoutItem.m_aStoredItems)
		{
			array<Managed> outComponents();
			slotEntity.FindComponents(BaseInventoryStorageComponent, outComponents);

			foreach (EL_DefaultLoadoutItem storedItem : loadoutItem.m_aStoredItems)
			{
				for (int i = 0; i < storedItem.m_iAmount; i++)
				{
					IEntity spawnedItem = SpawnDefaultCharacterItem(storageManager, storedItem);

					foreach (Managed componentRef : outComponents)
					{
						BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(componentRef);
						if (storageComponent.GetPurpose() & storedItem.m_ePurpose)
						{
							if (!storageManager.TryInsertItemInStorage(spawnedItem, storageComponent)) continue;

							InventoryItemComponent inventoryItemComponent = InventoryItemComponent.Cast(spawnedItem.FindComponent(InventoryItemComponent));
							if (inventoryItemComponent && !inventoryItemComponent.GetParentSlot()) continue;

							break;
						}
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

[BaseContainerProps()]
class EL_DefaultLoadoutItem
{
	[Attribute()]
	ResourceName m_rPrefab;

	[Attribute(defvalue: "1")]
	int m_iAmount;

	[Attribute(defvalue: EStoragePurpose.PURPOSE_DEPOSIT.ToString(), uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EStoragePurpose))]
	EStoragePurpose m_ePurpose;

	[Attribute()]
	ref array<ref EL_DefaultLoadoutItemComponent> m_aComponentDefaults;
	
	[Attribute()]
	ref array<ref EL_DefaultLoadoutItem> m_aStoredItems;
}

[BaseContainerProps()]
class EL_DefaultLoadoutItemComponent
{
	void ApplyTo(IEntity item);
}
