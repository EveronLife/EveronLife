[EL_ComponentSaveDataType(BaseSlotComponent), BaseContainerProps()]
class EL_BaseSlotComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName.Automatic()]
class EL_BaseSlotComponentSaveData : EL_ComponentSaveData
{
	ref EL_EntitySaveData m_pEntity;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseSlotComponent slot = BaseSlotComponent.Cast(component);
		IEntity slotEntity = slot.GetAttachedEntity();
		ResourceName prefab = EL_Utils.GetPrefabName(slotEntity);
		
		EL_EntitySlotPrefabInfo prefabInfo = EL_EntitySlotPrefabInfo.GetSlotInfo(owner, slot);
		bool isPrefabMatch = prefab == prefabInfo.GetEnabledSlotPrefab();

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

		// Read transform to see if slot uses OverrideTransformLS set.
		// Reuse logic from slot manager that should be used instead of base slot anyway ...
		EL_SlotManagerComponentSaveData.ReadTransform(slotEntity, saveData, prefabInfo, readResult);

		// We can safely ignore baked objects with default info on them, but anything else needs to be saved.
		if (attributes.m_bTrimDefaults &&
			isPrefabMatch &&
			EL_BitFlags.CheckFlags(slotPersistence.GetFlags(), EL_EPersistenceFlags.BAKED) &&
			readResult == EL_EReadResult.DEFAULT)
		{
			return EL_EReadResult.DEFAULT;
		}

		m_pEntity = saveData;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseSlotComponent slot = BaseSlotComponent.Cast(component);
		return EL_SlotManagerComponentSaveData.ApplySlot(slot.GetSlotInfo(), m_pEntity);
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_BaseSlotComponentSaveData otherData = EL_BaseSlotComponentSaveData.Cast(other);
		return (!m_pEntity && !otherData.m_pEntity) || m_pEntity.Equals(otherData.m_pEntity);
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;

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
