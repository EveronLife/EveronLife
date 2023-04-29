[EL_ComponentSaveDataType(EL_BaseInventoryStorageComponentSaveDataClass, BaseInventoryStorageComponent), BaseContainerProps()]
class EL_BaseInventoryStorageComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName(EL_BaseInventoryStorageComponentSaveData, "InventoryStorage")]
class EL_BaseInventoryStorageComponentSaveData : EL_ComponentSaveData
{
	int m_iPriority;
	EStoragePurpose m_ePurposeFlags;
	ref array<ref EL_PersistentInventoryStorageSlot> m_aSlots;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);

		m_iPriority = storageComponent.GetPriority();
		m_ePurposeFlags = storageComponent.GetPurpose();

		m_aSlots = {};

		bool isDefault = true;
		array<string> defaultPrefabs = GetDefaultPrefabs(storageComponent);

		for (int nSlot = 0, slots = storageComponent.GetSlotsCount(); nSlot < slots; nSlot++)
		{
			EL_PersistenceComponent slotPersistenceComponent = EL_Component<EL_PersistenceComponent>.Find(storageComponent.Get(nSlot));
			if (!slotPersistenceComponent) continue;

			EL_EReadResult readResult;
			EL_EntitySaveData saveData = slotPersistenceComponent.Save(readResult);
			if (!saveData)
				return EL_EReadResult.ERROR;

			// Reset transformation data, as that won't be needed for stored entites
			saveData.m_pTransformation.Reset();

			// Remove GarbageManager lifetime until the game fixes it being known for child entities some day.
			saveData.m_fRemainingLifetime = 0;

			EL_PersistentInventoryStorageSlot storageSlot();
			storageSlot.m_iSlotId = nSlot;
			storageSlot.m_pEntity = saveData;

			if ((readResult == EL_EReadResult.DEFAULT) &&
				(defaultPrefabs && !defaultPrefabs.IsEmpty()) &&
				EL_BitFlags.CheckFlags(slotPersistenceComponent.GetFlags(), EL_EPersistenceFlags.BAKED_PREFAB_CHILD) &&
				defaultPrefabs.Contains(saveData.m_rPrefab))
			{
				defaultPrefabs.RemoveItem(saveData.m_rPrefab);
			}
			else
			{
				isDefault = false;
			}

			m_aSlots.Insert(storageSlot);
		}

		// If any default prefabs are left over, it means some default items are missing -> can not be default
		if (isDefault && (!defaultPrefabs || defaultPrefabs.IsEmpty())) return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	protected array<string> GetDefaultPrefabs(BaseInventoryStorageComponent storageComponent)
	{
		return EL_PersistencePrefabInfo.GetPrefabChildren(storageComponent);
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

		set<int> processedSlots();
		foreach (EL_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			processedSlots.Insert(slot.m_iSlotId);

			// Try to use existing items in storage that are part of the prefab
			IEntity slotEntity = storageComponent.Get(slot.m_iSlotId);
			if (EL_Utils.GetPrefabName(slotEntity) == slot.m_pEntity.m_rPrefab)
			{
				EL_PersistenceComponent persistenceComponent = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
				if (!persistenceComponent || !persistenceComponent.Load(slot.m_pEntity, false))
				{
					storageManager.TryDeleteItem(slotEntity);
				}

				continue;
			}

			// Slot did not match save-data, delete current entity on it
			storageManager.TryDeleteItem(slotEntity);

			// Spawn new entity and insert it if not part of the prefab
			slotEntity = slot.m_pEntity.Spawn(false);
			if (!slotEntity)
				continue;

			if (storageManager.TryInsertItemInStorage(slotEntity, storageComponent, slot.m_iSlotId))
			{
				// Unable to add it to the storage parent, so put it on the ground at the parent origin
				EL_Utils.Teleport(slotEntity, storageComponent.GetOwner().GetOrigin(), storageComponent.GetOwner().GetYawPitchRoll()[0]);
			}

			processedSlots.Insert(slot.m_iSlotId);
		}

		// Remove all remaining prefab items
		for (int nSlot = 0, slots = storageComponent.GetSlotsCount(); nSlot < slots; nSlot++)
		{
			if (processedSlots.Contains(nSlot))
				continue;

			storageManager.TryDeleteItem(storageComponent.Get(nSlot));
		}

		return true;
	}
};

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
};
