[ComponentEditorProps(category: "EveronLife/Feature/Character/Spawning", description: "Part of the re-spawn system on the gamemode.")]
class EL_RespawnSytemComponentClass : SCR_RespawnSystemComponentClass
{
};

class EL_RespawnSytemComponent : SCR_RespawnSystemComponent
{
	[Attribute(category: "New character defaults")]
	protected ref array<ResourceName> m_aDefaultCharacterPrefabs;

	[Attribute(category: "New character defaults")]
	protected ref array<ref EL_DefaultLoadoutItem> m_aDefaultCharacterItems;

	protected ref map<GenericEntity, int> m_mLoadingCharacters = new map<GenericEntity, int>();
	protected ref map<int, GenericEntity> m_mPerparedCharacters = new map<int, GenericEntity>();

	//------------------------------------------------------------------------------------------------
	void PrepareCharacter(int playerId, EPF_CharacterSaveData saveData)
	{
		GenericEntity playerEntity;

		EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetFromCache(playerId);
		EL_PlayerCharacter activeCharacter = account.GetActiveCharacter();

		if (saveData)
		{
			saveData.m_pTransformation.m_bApplied = true;

			vector origin;
			if (!EPF_Const.IsNan(saveData.m_pTransformation.m_vOrigin))
				origin = saveData.m_pTransformation.m_vOrigin;

			// Spawn angle order is different for what ever reason
			vector spawnAngles;
			if (!EPF_Const.IsNan(saveData.m_pTransformation.m_vAngles))
				spawnAngles = Vector(saveData.m_pTransformation.m_vAngles[1], saveData.m_pTransformation.m_vAngles[0], saveData.m_pTransformation.m_vAngles[2]);

			playerEntity = DoSpawn(activeCharacter.GetPrefab(), origin, spawnAngles);

			EPF_PersistenceComponent persistenceComponent = EL_Component<EPF_PersistenceComponent>.Find(playerEntity);
			if (persistenceComponent)
			{
				// Remember which entity was for what player id
				m_mLoadingCharacters.Set(playerEntity, playerId);

				persistenceComponent.GetOnAfterLoadEvent().Insert(OnCharacterLoaded);
				if (persistenceComponent.Load(saveData))
					return;

				// On failure remove again
				persistenceComponent.GetOnAfterLoadEvent().Remove(OnCharacterLoaded);
				m_mLoadingCharacters.Remove(playerEntity);
			}

			SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
			playerEntity = null;
		}

		if (!playerEntity)
		{
			ResourceName charPrefab;

			if (activeCharacter)
			{
				charPrefab = activeCharacter.GetPrefab();
			}
			else
			{
				charPrefab = m_aDefaultCharacterPrefabs.GetRandomElement();
				if (!charPrefab)
				{
					Print("Could not create new character, no default prefabs configured. Go to EL_GameModeRoleplay > EL_RespawnSytemComponent and add at least one.", LogLevel.ERROR);
					return;
				}

				activeCharacter = EL_PlayerCharacter.Create(charPrefab);
				account.AddCharacter(activeCharacter, true);
			}

			vector position;
			vector angles;

			EL_SpawnPoint spawnPoint = EL_SpawnPoint.GetDefaultSpawnPoint();
			if (!spawnPoint)
			{
				Print("Could not spawn character, no default spawn point configured.", LogLevel.ERROR);
				return;
			}

			spawnPoint.GetPosAngles(position, angles);

			playerEntity = DoSpawn(charPrefab, position, angles);

			InventoryStorageManagerComponent storageManager = EL_Component<InventoryStorageManagerComponent>.Find(playerEntity);
			foreach (EL_DefaultLoadoutItem loadoutItem : m_aDefaultCharacterItems)
			{
				if (loadoutItem.m_ePurpose != EStoragePurpose.PURPOSE_LOADOUT_PROXY)
				{
					Debug.Error(string.Format("Failed to add '%1' as default character item. Only clothing/backpack/vest etc. with purpose '%2' are allowed as top level entries.", loadoutItem.m_rPrefab, typename.EnumToString(EStoragePurpose, EStoragePurpose.PURPOSE_LOADOUT_PROXY)));
					continue;
				}

				IEntity slotEntity = SpawnDefaultCharacterItem(storageManager, loadoutItem);
				if (!slotEntity) continue;

				if (!storageManager.TryInsertItem(slotEntity, loadoutItem.m_ePurpose))
				{
					SCR_EntityHelper.DeleteEntityAndChildren(slotEntity);
				}
			}

			EPF_PersistenceComponent persistenceComponent = EL_Component<EPF_PersistenceComponent>.Find(playerEntity);
			if (persistenceComponent)
			{
				persistenceComponent.SetPersistentId(activeCharacter.GetId());
			}
			else
			{
				Print(string.Format("Could not create new character, prefab '%1' is missing component '%2'.", charPrefab, EPF_PersistenceComponent), LogLevel.ERROR);
				SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
				return;
			}

			m_mPerparedCharacters.Set(playerId, playerEntity);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void OnCharacterLoaded(EPF_PersistenceComponent persistenceComponent, EPF_EntitySaveData saveData)
	{
		// We only want to know this once
		persistenceComponent.GetOnAfterLoadEvent().Remove(OnCharacterLoaded);

		GenericEntity playerEntity = GenericEntity.Cast(persistenceComponent.GetOwner());
		int playerId = m_mLoadingCharacters.Get(playerEntity);
		m_mLoadingCharacters.Remove(playerEntity);

		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		SCR_CharacterInventoryStorageComponent inventoryStorage = EL_Component<SCR_CharacterInventoryStorageComponent>.Find(playerEntity);
		if (inventoryStorage)
		{
			EPF_CharacterInventoryStorageComponentSaveData charInventorySaveData = EPF_ComponentSaveDataGetter<EPF_CharacterInventoryStorageComponentSaveData>.GetFirst(saveData);
			if (charInventorySaveData && charInventorySaveData.m_aQuickSlotEntities)
			{
				array<RplId> quickBarRplIds();
				// Init with invalid ids
				int nQuickslots = inventoryStorage.GetQuickSlotItems().Count();
				quickBarRplIds.Reserve(nQuickslots);
				for (int i = 0; i < nQuickslots; i++)
				{
					quickBarRplIds.Insert(RplId.Invalid());
				}

				foreach (EPF_PersistentQuickSlotItem quickSlot : charInventorySaveData.m_aQuickSlotEntities)
				{
					IEntity slotEntity = persistenceManager.FindEntityByPersistentId(quickSlot.m_sEntityId);
					if (slotEntity && quickSlot.m_iIndex < quickBarRplIds.Count())
					{
						RplComponent replication = RplComponent.Cast(slotEntity.FindComponent(RplComponent));
						if (replication) quickBarRplIds.Set(quickSlot.m_iIndex, replication.Id());
					}
				}

				// Apply quick item slots serverside to avoid inital sync back from client with same data
				inventoryStorage.EPF_Rpc_UpdateQuickSlotItems(quickBarRplIds);

				SCR_RespawnComponent respawnComponent = SCR_RespawnComponent.Cast(GetGame().GetPlayerManager().GetPlayerRespawnComponent(playerId));
				respawnComponent.EPF_SetQuickBarItems(quickBarRplIds);
			}
		}

		m_mPerparedCharacters.Set(playerId, playerEntity);
		return; //Wait a few frame for character and weapon controller and gadgets etc to be setup
	}

	//------------------------------------------------------------------------------------------------
	bool IsReadyForSpawn(int playerId)
	{
		return m_mPerparedCharacters.Contains(playerId);
	}

	//------------------------------------------------------------------------------------------------
	protected override GenericEntity RequestSpawn(int playerId)
	{
		GenericEntity playerEntity = m_mPerparedCharacters.Get(playerId);
		if (playerEntity)
		{
			m_mPerparedCharacters.Remove(playerId);
			return playerEntity;
		}

		Debug.Error("Attempt to spawn a character that has not finished processing. IsReadyForSpawn was not checked?");
		return null;
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
};

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
};

[BaseContainerProps()]
class EL_DefaultLoadoutItemComponent
{
	void ApplyTo(IEntity item);
};
