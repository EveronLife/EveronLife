[ComponentEditorProps(category: "EveronLife/Feature/Character/Spawning", description: "Part of the re-spawn system on the gamemode.")]
class EL_RespawnSystemComponentClass : EPF_BaseRespawnSystemComponentClass
{
}

class EL_RespawnSystemComponent : EPF_BaseRespawnSystemComponent
{
	[Attribute(category: "New character defaults")]
	protected ref array<ResourceName> m_aDefaultCharacterPrefabs;

	[Attribute(category: "New character defaults")]
	protected ref array<ref EL_DefaultLoadoutItem> m_aDefaultCharacterItems;

	//------------------------------------------------------------------------------------------------
	/*protected --Hotfix for 1.0 DO NOT CALL THIS MANUALLY*/
	override void HandlePlayerLoad(Managed context)
	{
		Tuple2<int, string> characterContext = Tuple2<int, string>.Cast(context);
		EDF_DataCallbackSingle<EL_PlayerAccount> callback(this, "OnAccountLoaded", characterContext);
		EL_PlayerAccountManager.GetInstance().LoadAccountAsync(characterContext.param2, true, callback);
	}

	//------------------------------------------------------------------------------------------------
	//! Handles the account information found for the player
	/*protected --Hotfix for 1.0 DO NOT CALL THIS MANUALLY*/
	void OnAccountLoaded(EL_PlayerAccount account, Managed context)
	{
		Tuple2<int, string> characterContext = Tuple2<int, string>.Cast(context);

		EL_PlayerCharacter activeCharacter = account.GetActiveCharacter();
		bool hasCharData = activeCharacter != null;
		if (!hasCharData)
		{
			ResourceName prefab = m_aDefaultCharacterPrefabs.GetRandomElement();
			if (prefab)
			{
				activeCharacter = EL_PlayerCharacter.Create(prefab);
				account.AddCharacter(activeCharacter, true);
			}
			else
			{
				Print("Could not create new character, no default prefabs configured. Go to EL_GameModeRoleplay > EL_RespawnSytemComponent and add at least one.", LogLevel.ERROR);
				return;
			}
		}

		characterContext.param2 = activeCharacter.GetId();

		#ifndef WORKBENCH
		// New account, skip to new character spawn
		if	(!hasCharData)
		{
			OnCharacterDataLoaded(EDF_EDbOperationStatusCode.SUCCESS, null, characterContext);
			return;
		}
		#endif

		super.HandlePlayerLoad(characterContext);
	}

	//------------------------------------------------------------------------------------------------
	override protected void GetCreationPosition(int playerId, string characterPersistenceId, out vector position, out vector yawPitchRoll)
	{
		EPF_SpawnPoint spawnPoint = EPF_SpawnPoint.GetRandomSpawnPoint();
		if (!spawnPoint)
		{
			Print("Could not spawn character, no spawn point on the map.", LogLevel.ERROR);
			return;
		}

		spawnPoint.GetPosYPR(position, yawPitchRoll);
	}

	//------------------------------------------------------------------------------------------------
	override protected ResourceName GetCreationPrefab(int playerId, string characterPersistenceId)
	{
		EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetFromCache(playerId);
		return account.GetActiveCharacter().GetPrefab();
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnCharacterCreated(int playerId, string characterPersistenceId, IEntity character)
	{
		InventoryStorageManagerComponent storageManager = EL_Component<InventoryStorageManagerComponent>.Find(character);
		foreach (EL_DefaultLoadoutItem loadoutItem : m_aDefaultCharacterItems)
		{
			if (loadoutItem.m_ePurpose != EStoragePurpose.PURPOSE_LOADOUT_PROXY)
			{
				Debug.Error(string.Format("Failed to add '%1' as default character item. Only clothing/backpack/vest etc. with purpose '%2' are allowed as top level entries.", loadoutItem.m_rPrefab, typename.EnumToString(EStoragePurpose, EStoragePurpose.PURPOSE_LOADOUT_PROXY)));
				continue;
			}

			IEntity slotEntity = SpawnDefaultCharacterItem(storageManager, loadoutItem);
			if (!slotEntity)
				continue;

			if (!storageManager.TryInsertItem(slotEntity, loadoutItem.m_ePurpose))
				SCR_EntityHelper.DeleteEntityAndChildren(slotEntity);
		}
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
}
