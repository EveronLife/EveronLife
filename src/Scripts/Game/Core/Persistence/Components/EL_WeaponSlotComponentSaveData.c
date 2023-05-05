[EL_ComponentSaveDataType(WeaponSlotComponent), BaseContainerProps()]
class EL_WeaponSlotComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName("WeaponSlot")]
class EL_WeaponSlotComponentSaveData : EL_ComponentSaveData
{
	int m_iSlotIndex;
	ref EL_EntitySaveData m_pEntity;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		WeaponSlotComponent slot = WeaponSlotComponent.Cast(component);
		IEntity slotEntity = slot.GetWeaponEntity();
		ResourceName prefab = EL_Utils.GetPrefabName(slotEntity);
		ResourceName slotPrefab = EL_EntitySlotPrefabInfo.GetSlotPrefab(owner, slot);
		bool isPrefabMatch = prefab == slotPrefab;

		EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
		if (!slotPersistence)
		{
			// Slot is different from prefab and the new slot has no persistence component so we clear it.
			if (!isPrefabMatch)
				return EL_EReadResult.OK;

			return EL_EReadResult.DEFAULT;
		}

		EL_EReadResult readResult;
		EL_EntitySaveData saveData = slotPersistence.Save(readResult);
		if (!saveData)
			return EL_EReadResult.ERROR;

		// Reset transformation data, as that won't be needed for slotted entites
		saveData.m_pTransformation.Reset();

		// Remove GarbageManager lifetime until the game fixes it being known for child entities some day.
		saveData.m_fRemainingLifetime = 0;

		// We can safely ignore baked objects with default info on them, but anything else needs to be saved.
		if (attributes.m_bTrimDefaults &&
			isPrefabMatch &&
			EL_BitFlags.CheckFlags(slotPersistence.GetFlags(), EL_EPersistenceFlags.BAKED) &&
			readResult == EL_EReadResult.DEFAULT)
		{
			return EL_EReadResult.DEFAULT;
		}

		m_iSlotIndex = slot.GetWeaponSlotIndex();
		m_pEntity = saveData;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool IsFor(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		WeaponSlotComponent slot = WeaponSlotComponent.Cast(component);
		return slot.GetWeaponSlotIndex() == m_iSlotIndex;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		WeaponSlotComponent slot = WeaponSlotComponent.Cast(component);
		IEntity slotEntity = slot.GetWeaponEntity();

		// Found matching entity, no need to spawn, just apply save-data
		if (m_pEntity &&
			slotEntity &&
			EL_Utils.GetPrefabName(slotEntity) == EL_EntitySlotPrefabInfo.GetSlotPrefab(owner, slot))
		{
			EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
			if (slotPersistence && !slotPersistence.Load(m_pEntity, false))
				return EL_EApplyResult.ERROR;

			return EL_EApplyResult.OK;
		}

		// Slot did not match save-data, delete current entity on it
		SCR_EntityHelper.DeleteEntityAndChildren(slotEntity);

		if (!m_pEntity)
			return EL_EApplyResult.OK;

		// Spawn new entity and attach it
		slotEntity = m_pEntity.Spawn(false);
		if (slotEntity)
		{
			slot.SetWeapon(slotEntity);
			if (slot.GetWeaponEntity() == slotEntity)
				return EL_EApplyResult.OK;
		}

		return EL_EApplyResult.ERROR;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_WeaponSlotComponentSaveData otherData = EL_WeaponSlotComponentSaveData.Cast(other);
		return (m_iSlotIndex == otherData.m_iSlotIndex) && ((!m_pEntity && !otherData.m_pEntity) || m_pEntity.Equals(otherData.m_pEntity));
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;

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
		if (!loadContext.IsValid())
			return false;

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
