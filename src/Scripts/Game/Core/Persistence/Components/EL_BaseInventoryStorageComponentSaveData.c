[EL_ComponentSaveDataType(EL_BaseInventoryStorageComponentSaveDataClass, BaseInventoryStorageComponent), BaseContainerProps()]
class EL_BaseInventoryStorageComponentSaveDataClass : EL_ComponentSaveDataClass
{
}

[EL_DbName(EL_BaseInventoryStorageComponentSaveData, "InventoryStorage")]
class EL_BaseInventoryStorageComponentSaveData : EL_ComponentSaveData
{
	int m_iPriority;
	EStoragePurpose m_ePurposeFlags;
	ref array<ref EL_PersistentInventoryStorageSlot> m_aSlots;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);

		m_iPriority = storageComponent.GetPriority();
		m_ePurposeFlags = storageComponent.GetPurpose();

		m_aSlots = new array<ref EL_PersistentInventoryStorageSlot>();

		for (int nSlot = 0, slots = storageComponent.GetSlotsCount(); nSlot < slots; nSlot++)
		{
			IEntity slotEntity = storageComponent.Get(nSlot);
			if (!slotEntity) continue;

			EL_PersistenceComponent slotPersistenceComponent = EL_PersistenceComponent.Cast(slotEntity.FindComponent(EL_PersistenceComponent));
			if (!slotPersistenceComponent) continue;

			EL_EntitySaveData saveData = slotPersistenceComponent.Save();
			if (!saveData) continue;

			// Reset transformation data, as that won't be needed for stored entites
			saveData.m_pTransformation.Reset();

			// Remove GarbageManager lifetime until the game fixes it being known for child entities some day.
			saveData.m_fRemainingLifetime = 0;

			EL_PersistentInventoryStorageSlot slotInfo();
			slotInfo.m_iSlotId = nSlot;
			slotInfo.m_pEntity = saveData;
			m_aSlots.Insert(slotInfo);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool IsFor(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);
		return (storageComponent.GetPriority() == m_iPriority) && (storageComponent.GetPurpose() == m_ePurposeFlags);
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);
		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(storageComponent.GetOwner().FindComponent(InventoryStorageManagerComponent));
		if (!storageManager) storageManager = EL_GlobalInventoryStorageManagerComponent.GetInstance();

		array<IEntity> prefabItems();
		storageComponent.GetAll(prefabItems);

		foreach (EL_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			// Try to use existing items in storage that are part of the prefab
			bool reUsedPrefabItem = false;
			foreach (IEntity prefabItem : prefabItems)
			{
				if (EL_Utils.GetPrefabName(prefabItem) == slot.m_pEntity.m_rPrefab)
				{
					EL_PersistenceComponent persistenceComponent = EL_Component<EL_PersistenceComponent>.Find(prefabItem);
					if (persistenceComponent && persistenceComponent.Load(slot.m_pEntity))
					{
						reUsedPrefabItem = true;
						prefabItems.RemoveItem(prefabItem);
						break;
					}
				}
			}
			if (reUsedPrefabItem) continue;

			// Spawn new entity and insert it if not part of the prefab
			IEntity slotEntity = slot.m_pEntity.Spawn();
			if (!slotEntity)
			{
				PrintFormat("Failed to spawn storage slot entity prefab '%1'.", slot.m_pEntity.m_rPrefab);
				continue;
			}

			if (storageManager.TryInsertItemInStorage(slotEntity, storageComponent, slot.m_iSlotId))
			{
				// Unable to add it to the storage parent, so put it on the ground at the parent origin
				EL_Utils.Teleport(slotEntity, storageComponent.GetOwner().GetOrigin(), storageComponent.GetOwner().GetYawPitchRoll()[0]);
			}
		}

		// Remove all remainin prefab items
		foreach (IEntity prefabItem : prefabItems)
		{
			storageManager.TryDeleteItem(prefabItem);
		}

		return true;
	}
}

class EL_PersistentInventoryStorageSlot
{
	int m_iSlotId;
	ref EL_EntitySaveData m_pEntity;

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		saveContext.WriteValue("m_iSlotId", m_iSlotId);
		saveContext.WriteValue("entityType", EL_DbName.Get(m_pEntity.Type()));
		saveContext.WriteValue("m_pEntity", m_pEntity);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		loadContext.ReadValue("m_iSlotId", m_iSlotId);

		string entityTypeString;
		loadContext.ReadValue("entityType", entityTypeString);
		typename entityType = EL_DbName.GetTypeByName(entityTypeString);
		if (!entityType) return false;

		m_pEntity = EL_EntitySaveData.Cast(entityType.Spawn());
		loadContext.ReadValue("m_pEntity", m_pEntity);

		return true;
	}
}
