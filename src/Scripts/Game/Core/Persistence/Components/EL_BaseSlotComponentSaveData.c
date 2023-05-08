[EL_ComponentSaveDataType(BaseSlotComponent), BaseContainerProps()]
class EL_BaseSlotComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName("SlotComponent")]
class EL_BaseSlotComponentSaveData : EL_ComponentSaveData
{
	ref EL_EntitySaveData m_pEntity;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseSlotComponent slot = BaseSlotComponent.Cast(component);
		IEntity slotEntity = slot.GetAttachedEntity();
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

		// Read transform to see if slot uses OverrideTransformLS set.
		EL_PersistenceComponentClass slotAttributes = EL_ComponentData<EL_PersistenceComponentClass>.Get(slotEntity);
		if (saveData.m_pTransformation.ReadFrom(slotEntity, slotAttributes.m_pSaveData))
			readResult = EL_EReadResult.OK;

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
		IEntity slotEntity = slot.GetAttachedEntity();

		// If there is an tramsform override saved we need to consume it before load operations
		EL_PersistentTransformation persistentTransform;
		if (m_pEntity)
		{
			persistentTransform = m_pEntity.m_pTransformation;
			m_pEntity.m_pTransformation = null;
		}

		// Found matching entity, no need to spawn, just apply save-data
		if (m_pEntity &&
			slotEntity &&
			EL_Utils.GetPrefabName(slotEntity) == EL_EntitySlotPrefabInfo.GetSlotPrefab(owner, slot))
		{
			EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
			if (slotPersistence && !slotPersistence.Load(m_pEntity, false))
				return EL_EApplyResult.ERROR;
		}
		else
		{
			// Slot did not match save-data, delete current entity on it
			SCR_EntityHelper.DeleteEntityAndChildren(slotEntity);

			if (!m_pEntity)
				return EL_EApplyResult.OK;

			// Spawn new entity and attach it
			slotEntity = m_pEntity.Spawn(false);
			if (!slotEntity)
				return EL_EApplyResult.ERROR;

			slot.GetSlotInfo().AttachEntity(slotEntity);
			if (slot.GetAttachedEntity() != slotEntity)
				return EL_EApplyResult.ERROR;
		}

		if (persistentTransform)
		{
			vector transform[4];

			if (persistentTransform.m_vOrigin != EL_Const.VEC_INFINITY)
				transform[3] = persistentTransform.m_vOrigin;

			if (persistentTransform.m_vAngles != EL_Const.VEC_INFINITY)
			{
				Math3D.AnglesToMatrix(persistentTransform.m_vAngles, transform);
			}
			else
			{
				Math3D.MatrixIdentity3(transform);
			}

			if (persistentTransform.m_fScale != float.INFINITY)
				SCR_Math3D.ScaleMatrix(transform, persistentTransform.m_fScale);

			slot.GetSlotInfo().OverrideTransformLS(transform);
		}

		return EL_EApplyResult.OK;
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
