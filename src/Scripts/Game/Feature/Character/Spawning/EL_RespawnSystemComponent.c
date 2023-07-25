[ComponentEditorProps(category: "EveronLife/Feature/Character/Spawning", description: "Part of the re-spawn system on the gamemode.")]
class EL_RespawnSystemComponentClass : SCR_RespawnSystemComponentClass
{
}

class EL_RespawnSystemComponent : SCR_RespawnSystemComponent
{
	[Attribute(category: "New character defaults")]
	protected ref array<ResourceName> m_aDefaultCharacterPrefabs;

	[Attribute(category: "New character defaults")]
	protected ref array<ref EL_DefaultLoadoutItem> m_aDefaultCharacterItems;

	protected ref map<int, IEntity> m_mLoadingCharacters = new map<int, IEntity>();

	protected PlayerManager m_pPlayerManager;

	//------------------------------------------------------------------------------------------------
	override void OnPlayerRegistered_S(int playerId)
	{
		//PrintFormat("EPF_BasicRespawnSystemComponent.OnPlayerRegistered_S(%1)", playerId);

		if (RplSession.Mode() != RplMode.Dedicated)
		{
			WaitForUid(playerId);
		}
		else
		{
			EDF_ScriptInvokerCallback1<int> callback(this, "WaitForUid");
			m_pGameMode.GetOnPlayerAuditSuccess().Insert(callback.Invoke)
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled_S(int playerId, IEntity player, IEntity killer)
	{
		//PrintFormat("EPF_BasicRespawnSystemComponent.OnPlayerKilled_S(%1, %2, %3)", playerId, player, killer);

		// Add the dead body root entity collection so it spawns back after restart for looting
		EPF_PersistenceComponent persistence = EPF_Component<EPF_PersistenceComponent>.Find(player);
		if (!persistence)
		{
			Print(string.Format("OnPlayerKilled(%1, %2, %3) -> Player killed that does not have persistence component?!? Something went terribly wrong!", playerId, player, killer), LogLevel.ERROR);
			return;
		}

		string newId = persistence.GetPersistentId();

		persistence.SetPersistentId(string.Empty); // Force generation of new id for dead body
		persistence.OverrideSelfSpawn(true);

		// Fresh character spawn (NOTE: We need to push this to next frame due to a bug where on the same death frame we can not hand over a new char).
		GetGame().GetCallqueue().Call(LoadCharacter, playerId, null);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerDisconnected_S(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity character = m_pPlayerManager.GetPlayerController(playerId).GetControlledEntity();
		if (character)
		{
			EPF_PersistenceComponent persistence = EPF_Component<EPF_PersistenceComponent>.Find(character);
			persistence.PauseTracking();
			persistence.Save();
			// Game will cleanup the char for us because it was controlled by the player.
		}
		else
		{
			// Delete a still loading char that was not handed over to a player.
			IEntity transientCharacter = m_mLoadingCharacters.Get(playerId);
			EPF_PersistenceComponent persistence = EPF_Component<EPF_PersistenceComponent>.Find(transientCharacter);
			if (persistence)
				persistence.PauseTracking();

			SCR_EntityHelper.DeleteEntityAndChildren(transientCharacter);
		}

		EL_PlayerAccountManager accountManager = EL_PlayerAccountManager.GetInstance();
		EL_PlayerAccount account = accountManager.GetFromCache(playerId);
		if (!account)
			return;

		if (account)
			accountManager.SaveAndReleaseAccount(account);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerDeleted_S(int playerId)
	{
		// Remove base implementation
	}

	//------------------------------------------------------------------------------------------------
	protected void WaitForUid(int playerId)
	{
		// Wait one frame after audit/sp join, then it is available.
		// TODO: Remove this method once https://feedback.bistudio.com/T165590 is fixed.
		GetGame().GetCallqueue().Call(OnUidAvailable, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnUidAvailable(int playerId)
	{
		Tuple2<int, string> characterContext(playerId, EPF_Utils.GetPlayerUID(playerId));

		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		if (persistenceManager.GetState() < EPF_EPersistenceManagerState.ACTIVE)
		{
			// Wait with character load until the persistence system is fully loaded
			EDF_ScriptInvokerCallback callback(this, "RequestAccountLoad", characterContext);
			persistenceManager.GetOnActiveEvent().Insert(callback.Invoke);
			return;
		}

		RequestAccountLoad(characterContext);
	}

	//------------------------------------------------------------------------------------------------
	protected void RequestAccountLoad(Managed context)
	{
		Tuple2<int, string> characterContext = Tuple2<int, string>.Cast(context);
		EDF_DataCallbackSingle<EL_PlayerAccount> callback(this, "OnAccountLoaded", characterContext);
		EL_PlayerAccountManager.GetInstance().LoadAccountAsync(characterContext.param2, true, callback);
	}

	//------------------------------------------------------------------------------------------------
	//! Handles the account information found for the player
	protected void OnAccountLoaded(EL_PlayerAccount account, Managed context)
	{
		Tuple2<int, string> playerInfo = Tuple2<int, string>.Cast(context);

		EL_PlayerCharacter activeCharacter = account.GetActiveCharacter();

		if (!activeCharacter)
		{
			// New account, skip to new character spawn
			LoadCharacter(playerInfo.param1, null);
			return;
		}

		// Load first available character until selection flow is implemented
		string characterId = activeCharacter.GetId();
		Tuple2<int, string> characterContext(playerInfo.param1, playerInfo.param2);
		EDF_DbFindCallbackSingle<EPF_CharacterSaveData> characterDataCallback(this, "OnCharacterDataLoaded", characterContext);
		EPF_PersistenceEntityHelper<EPF_CharacterSaveData>.GetRepository().FindAsync(characterId, characterDataCallback);
	}

	//------------------------------------------------------------------------------------------------
	//! Handles the character data found for the players account
	protected void OnCharacterDataLoaded(EDF_EDbOperationStatusCode statusCode, EPF_CharacterSaveData characterData, Managed context)
	{
		/*
			TODO: Use respawn component system for pre spawn communication with player so they can choose the char to spawn with and where potentially
				- Component is on the player controller and is allowed to send rpcs to server (s. SCR_RespawnComponent for example setup)
				- Spawn on last location (if previously spawned)
				- Select spawns on map (depending on which job is active to enable things such as police spawns)
					- Maybe list with addtional spawns per job + city spawns every person has.
				- different position would just need to override the saveData transformation component data.
		*/

		Tuple2<int, string> characterInfo = Tuple2<int, string>.Cast(context);
		LoadCharacter(characterInfo.param1, characterData);
	}

	//------------------------------------------------------------------------------------------------
	protected void LoadCharacter(int playerId, EPF_CharacterSaveData saveData)
	{
		IEntity playerEntity;

		if (saveData)
		{
			vector position = saveData.m_pTransformation.m_vOrigin;
			SCR_WorldTools.FindEmptyTerrainPosition(position, position, 2);
			saveData.m_pTransformation.m_vOrigin = position + "0 0.1 0"; // Anti lethal terrain clipping

			playerEntity = saveData.Spawn();

			EPF_PersistenceComponent persistenceComponent = EPF_Component<EPF_PersistenceComponent>.Find(playerEntity);
			if (persistenceComponent)
			{
				m_mLoadingCharacters.Set(playerId, playerEntity);

				if (EPF_DeferredApplyResult.IsPending(saveData))
				{
					Tuple3<int, EPF_CharacterSaveData, EPF_PersistenceComponent> context(playerId, saveData, persistenceComponent);
					EDF_ScriptInvokerCallback callback(this, "OnCharacterLoadedCallback", context);
					persistenceComponent.GetOnAfterLoadEvent().Insert(callback.Invoke);

					// TODO: Remove hard loading time limit when we know all spawn block bugs are fixed.
					GetGame().GetCallqueue().CallLater(OnCharacterLoaded, 5000, false, playerId, saveData, persistenceComponent);
				}
				else
				{
					OnCharacterLoaded(playerId, saveData, persistenceComponent);
				}

				return;
			}
		}

		EL_PlayerAccount account = EL_PlayerAccountManager.GetInstance().GetFromCache(playerId);
		EL_PlayerCharacter activeCharacter = account.GetActiveCharacter();
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

		EPF_SpawnPoint spawnPoint = EPF_SpawnPoint.GetRandomSpawnPoint();
		if (!spawnPoint)
		{
			Print("Could not spawn character, no spawn point on the map.", LogLevel.ERROR);
			return;
		}

		vector position, ypr;
		spawnPoint.GetPosYPR(position, ypr);

		playerEntity = EPF_Utils.SpawnEntityPrefab(charPrefab, position + "0 0.1 0", ypr);
		m_mLoadingCharacters.Set(playerId, playerEntity);

		InventoryStorageManagerComponent storageManager = EL_Component<InventoryStorageManagerComponent>.Find(playerEntity);
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

		EPF_PersistenceComponent persistenceComponent = EPF_Component<EPF_PersistenceComponent>.Find(playerEntity);
		if (persistenceComponent)
		{
			persistenceComponent.SetPersistentId(activeCharacter.GetId());
			HandoverToPlayer(playerId, playerEntity);
		}
		else
		{
			Print(string.Format("Could not create new character, prefab '%1' is missing component '%2'.", charPrefab, EPF_PersistenceComponent), LogLevel.ERROR);
			SCR_EntityHelper.DeleteEntityAndChildren(playerEntity);
			return;
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void OnCharacterLoadedCallback(Managed context)
	{
		auto typedContext = Tuple3<int, EPF_CharacterSaveData, EPF_PersistenceComponent>.Cast(context);
		OnCharacterLoaded(typedContext.param1, typedContext.param2, typedContext.param3);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnCharacterLoaded(int playerId, EPF_EntitySaveData saveData, EPF_PersistenceComponent persistenceComponent)
	{
		if (!persistenceComponent || !saveData)
			return;

		// We only want to know this once
		persistenceComponent.GetOnAfterLoadEvent().Remove(OnCharacterLoaded);

		IEntity playerEntity = persistenceComponent.GetOwner();
		if (m_pPlayerManager.GetPlayerControlledEntity(playerId) == playerEntity)
			return; // Player was force taken over after the time limit

		EPF_PersistenceManager persistenceManager = EPF_PersistenceManager.GetInstance();
		SCR_CharacterInventoryStorageComponent inventoryStorage = EPF_Component<SCR_CharacterInventoryStorageComponent>.Find(playerEntity);
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
						quickBarRplIds.Set(quickSlot.m_iIndex, EPF_NetworkUtils.GetRplId(slotEntity));
					}
				}

				// Apply quick item slots serverside to avoid initial sync back from client with same data
				inventoryStorage.EPF_Rpc_UpdateQuickSlotItems(quickBarRplIds);

				// Send quickbar to client on next frame when ownership was handed over
				GetGame().GetCallqueue().Call(inventoryStorage.EPF_SetQuickBarItems, quickBarRplIds);
			}
		}

		HandoverToPlayer(playerId, playerEntity);
	}

	//------------------------------------------------------------------------------------------------
	protected void HandoverToPlayer(int playerId, IEntity character)
	{
		//PrintFormat("HandoverToPlayer(%1, %2)", playerId, character);
		SCR_PlayerController playerController = SCR_PlayerController.Cast(m_pPlayerManager.GetPlayerController(playerId));
		EDF_ScriptInvokerCallback callback(this, "OnHandoverComplete", new Tuple1<int>(playerId));
		playerController.m_OnControlledEntityChanged.Insert(callback.Invoke);

		playerController.SetInitialMainEntity(character);
		
		m_pGameMode.OnPlayerEntityChanged_S(playerId, null, character);

		SCR_RespawnComponent respawn = SCR_RespawnComponent.Cast(playerController.GetRespawnComponent());
		respawn.SGetOnSpawn().Invoke(); // TODO: Check if this is needed, the base game added it as a hack?!?
		respawn.NotifySpawn(character);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnHandoverComplete(Managed context)
	{
		Tuple1<int> typedContext = Tuple1<int>.Cast(context);
		//PrintFormat("OnHandoverComplete(%1)", typedContext.param1);
		m_mLoadingCharacters.Remove(typedContext.param1);
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
		m_pGameMode = SCR_BaseGameMode.Cast(owner);
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_pPlayerManager = GetGame().GetPlayerManager();
		if (!m_pGameMode || !m_pRplComponent || !m_pPlayerManager)
			Debug.Error("SCR_RespawnSystemComponent setup is invalid!");
	}
}
