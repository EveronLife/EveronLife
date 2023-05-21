[EL_ComponentSaveDataType(BaseInventoryStorageComponent), BaseContainerProps()]
class EL_BaseInventoryStorageComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName.Automatic()]
class EL_BaseInventoryStorageComponentSaveData : EL_ComponentSaveData
{
	int m_iPriority;
	EStoragePurpose m_ePurposeFlags;
	ref array<ref EL_PersistentInventoryStorageSlot> m_aSlots;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(component);

		m_iPriority = storageComponent.GetPriority();
		m_ePurposeFlags = storageComponent.GetPurpose();
		m_aSlots = {};

		for (int nSlot = 0, slots = storageComponent.GetSlotsCount(); nSlot < slots; nSlot++)
		{
			IEntity slotEntity = storageComponent.Get(nSlot);
			ResourceName prefab = EL_Utils.GetPrefabName(slotEntity);
			ResourceName defaultSlotPrefab = EL_DefaultPrefabItemsInfo.GetDefaultPrefab(storageComponent, nSlot);

			EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
			if (!slotPersistence)
			{
				// Storage normally has a default prefab spawned onto this slot idx, but it is not persistent so it needs to be removed.
				if (defaultSlotPrefab)
				{
					EL_PersistentInventoryStorageSlot persistentSlot();
					persistentSlot.m_iSlotIndex = nSlot;
					m_aSlots.Insert(persistentSlot);
				}

				continue;
			}

			EL_EReadResult readResult;
			EL_EntitySaveData saveData = slotPersistence.Save(readResult);
			if (!saveData)
				return EL_EReadResult.ERROR;

			// We can safely ignore baked objects with default info on them, but anything else needs to be saved.
			if (attributes.m_bTrimDefaults &&
				prefab == defaultSlotPrefab &&
				EL_BitFlags.CheckFlags(slotPersistence.GetFlags(), EL_EPersistenceFlags.BAKED) &&
				readResult == EL_EReadResult.DEFAULT)
			{
				continue;
			}

			EL_PersistentInventoryStorageSlot persistentSlot();
			persistentSlot.m_iSlotIndex = nSlot;
			persistentSlot.m_pEntity = saveData;
			m_aSlots.Insert(persistentSlot);
		}

		if (m_aSlots.IsEmpty())
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool IsFor(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(component);
		return (storageComponent.GetPriority() == m_iPriority) && (storageComponent.GetPurpose() == m_ePurposeFlags);
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(component);
		InventoryStorageManagerComponent storageManager = InventoryStorageManagerComponent.Cast(storageComponent.GetOwner().FindComponent(InventoryStorageManagerComponent));
		if (!storageManager) storageManager = EL_GlobalInventoryStorageManagerComponent.GetInstance();

		foreach (EL_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			IEntity slotEntity = storageComponent.Get(slot.m_iSlotIndex);

			// Found matching entity, no need to spawn, just apply save-data
			if (slot.m_pEntity &&
				slotEntity &&
				EL_Utils.GetPrefabName(slotEntity) == EL_DefaultPrefabItemsInfo.GetDefaultPrefab(storageComponent, slot.m_iSlotIndex))
			{
				EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
				if (slotPersistence && !slotPersistence.Load(slot.m_pEntity, false))
					return EL_EApplyResult.ERROR;

				continue;
			}

			// Slot did not match save-data, delete current entity on it
			storageManager.TryDeleteItem(slotEntity);

			if (!slot.m_pEntity)
				continue;

			// Spawn new entity and attach it
			slotEntity = slot.m_pEntity.Spawn(false);
			if (!slotEntity)
				return EL_EApplyResult.ERROR;

			// Unable to add it to the storage parent, so put it on the ground at the parent origin
			if (!storageManager.TryInsertItemInStorage(slotEntity, storageComponent, slot.m_iSlotIndex))
				EL_Utils.Teleport(slotEntity, storageComponent.GetOwner().GetOrigin(), storageComponent.GetOwner().GetYawPitchRoll()[0]);
		}

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_BaseInventoryStorageComponentSaveData otherData = EL_BaseInventoryStorageComponentSaveData.Cast(other);

		if (m_iPriority != otherData.m_iPriority ||
			m_ePurposeFlags != otherData.m_ePurposeFlags ||
			m_aSlots.Count() != otherData.m_aSlots.Count())
			return false;

		foreach (int idx, EL_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			// Try same index first as they are likely to be the correct ones.
			if (slot.Equals(otherData.m_aSlots.Get(idx)))
				continue;

			bool found;
			foreach (int compareIdx, EL_PersistentInventoryStorageSlot otherSlot : otherData.m_aSlots)
			{
				if (compareIdx == idx)
					continue; // Already tried in idx direct compare

				if (slot.Equals(otherSlot))
				{
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}

		return true;
	}
};

class EL_PersistentInventoryStorageSlot
{
	int m_iSlotIndex;
	ref EL_EntitySaveData m_pEntity;

	//------------------------------------------------------------------------------------------------
	bool Equals(notnull EL_PersistentInventoryStorageSlot other)
	{
		return m_iSlotIndex == other.m_iSlotIndex && m_pEntity.Equals(other.m_pEntity);
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		saveContext.WriteValue("m_iSlotIndex", m_iSlotIndex);

		string entityType = "EMPTY";
		if (m_pEntity)
			entityType = EL_DbName.Get(m_pEntity.Type());

		saveContext.WriteValue("entityType", entityType);

		if (entityType)
			saveContext.WriteValue("m_pEntity", m_pEntity);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		loadContext.ReadValue("m_iSlotIndex", m_iSlotIndex);

		string entityTypeString;
		loadContext.ReadValue("entityType", entityTypeString);

		if (entityTypeString == "EMPTY")
			return true;

		typename entityType = EL_DbName.GetTypeByName(entityTypeString);
		if (!entityType)
			return false;

		m_pEntity = EL_EntitySaveData.Cast(entityType.Spawn());
		loadContext.ReadValue("m_pEntity", m_pEntity);

		return true;
	}
};
