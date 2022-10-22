[EL_ComponentSaveDataType(EL_BaseInventoryStorageComponentSaveData, BaseInventoryStorageComponent, "InventoryStorage"), BaseContainerProps()]
class EL_BaseInventoryStorageComponentSaveData : EL_ComponentSaveDataBase
{
	int m_iPriority;
	EStoragePurpose m_ePurposeFlags;
	ref array<ref EL_PersistentInventoryStorageSlot> m_aSlots;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);

		m_iPriority = storageComponent.GetPriority();
		m_ePurposeFlags = storageComponent.GetPurpose();

		m_aSlots = new array<ref EL_PersistentInventoryStorageSlot>();

		for (int i = 0, nSlot = 0, slots = storageComponent.GetSlotsCount(); i < slots; i++)
		{
			IEntity slotEntity = storageComponent.Get(i);
			if (!slotEntity) continue;

			EL_PersistenceComponent slotPersistenceComponent = EL_PersistenceComponent.Cast(slotEntity.FindComponent(EL_PersistenceComponent));
			if (!slotPersistenceComponent) continue;

			EL_EntitySaveDataBase saveData = slotPersistenceComponent.Save();
			if (!saveData) continue;

			// Remove transformation data, as that won't be needed for stored entites
			saveData.m_mComponentsSaveData.Remove(EL_TransformationSaveData);

			EL_PersistentInventoryStorageSlot slotInfo();
			slotInfo.m_iSlotId = nSlot++;
			slotInfo.m_pEntity = saveData;
			m_aSlots.Insert(slotInfo)
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool IsFor(notnull GenericComponent worldEntityComponent)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);
		return (storageComponent.GetPriority() == m_iPriority) && (storageComponent.GetPurpose() == m_ePurposeFlags);
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);
		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(storageComponent.GetOwner().FindComponent(InventoryStorageManagerComponent));
		if (!storageManager) storageManager = EL_GlobalInventoryStorageManagerComponent.GetInstance();

		array<IEntity> prefabItems();
		storageComponent.GetAll(prefabItems);
		foreach (IEntity prefabItem : prefabItems)
		{
			storageManager.TryDeleteItem(prefabItem);
		}

		foreach (EL_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			IEntity slotEntity = slot.m_pEntity.Spawn();
			if (!slotEntity)
			{
				PrintFormat("Failed to spawn storage slot entity prefab '%1'.", slot.m_pEntity.m_rPrefab);
				continue;
			}

			if (m_ePurposeFlags & EStoragePurpose.PURPOSE_LOADOUT_PROXY)
			{
				BaseLoadoutManagerComponent loadout = BaseLoadoutManagerComponent.Cast(storageComponent.GetOwner().FindComponent(BaseLoadoutManagerComponent));
				loadout.Wear(slotEntity);
			}
			else
			{
				storageManager.TryInsertItemInStorage(slotEntity, storageComponent, slot.m_iSlotId);
			}

			InventoryItemComponent inventoryItemComponent = InventoryItemComponent.Cast(slotEntity.FindComponent(InventoryItemComponent));
			if (inventoryItemComponent && !inventoryItemComponent.GetParentSlot())
			{
				// Unable to add it to the storage parent, so put it on the ground at the parent origin
				EL_Utils.Teleport(slotEntity, storageComponent.GetOwner().GetOrigin(), storageComponent.GetOwner().GetYawPitchRoll());
			}
		}

		return true;
	}
}

class EL_PersistentInventoryStorageSlot
{
	int m_iSlotId;
	ref EL_EntitySaveDataBase m_pEntity;

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

		m_pEntity = EL_EntitySaveDataBase.Cast(entityType.Spawn());
		loadContext.ReadValue("m_pEntity", m_pEntity);

		return true;
	}
}
