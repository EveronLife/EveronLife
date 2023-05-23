[EL_ComponentSaveDataType(SlotManagerComponent), BaseContainerProps()]
class EL_SlotManagerComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName.Automatic()]
class EL_SlotManagerComponentSaveData : EL_ComponentSaveData
{
	ref array<ref EL_PersistentEntitySlot> m_aSlots;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		SlotManagerComponent slotManager = SlotManagerComponent.Cast(component);

		m_aSlots = {};
		array<ref EL_EntitySlotPrefabInfo> slotinfos = EL_EntitySlotPrefabInfo.GetSlotInfos(owner, slotManager);

		array<EntitySlotInfo> outSlotInfos();
		slotManager.GetSlotInfos(outSlotInfos);
		foreach (int idx, EntitySlotInfo entitySlot : outSlotInfos)
		{
			IEntity slotEntity = entitySlot.GetAttachedEntity();
			ResourceName prefab = EL_Utils.GetPrefabName(slotEntity);

			EL_EntitySlotPrefabInfo prefabInfo = slotinfos.Get(idx);
			bool isPrefabMatch = prefab == prefabInfo.GetEnabledSlotPrefab();

			EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
			if (!slotPersistence)
			{
				// Slot is different from prefab and the new slot has no persistence component so we clear it.
				if (!isPrefabMatch)
				{
					EL_PersistentEntitySlot persistentSlot();
					persistentSlot.m_sName = prefabInfo.m_sName;
					m_aSlots.Insert(persistentSlot);
				}

				continue;
			}

			EL_EReadResult readResult;
			EL_EntitySaveData saveData = slotPersistence.Save(readResult);
			if (!saveData)
				return EL_EReadResult.ERROR;

			// Read transform to see if slot uses OverrideTransformLS set.
			ReadTransform(slotEntity, saveData, prefabInfo, readResult);

			// We can safely ignore baked objects with default info on them, but anything else needs to be saved.
			if (attributes.m_bTrimDefaults &&
				isPrefabMatch &&
				EL_BitFlags.CheckFlags(slotPersistence.GetFlags(), EL_EPersistenceFlags.BAKED) &&
				readResult == EL_EReadResult.DEFAULT)
			{
				continue;
			}

			EL_PersistentEntitySlot persistentSlot();
			persistentSlot.m_sName = prefabInfo.m_sName;
			persistentSlot.m_pEntity = saveData;
			m_aSlots.Insert(persistentSlot);
		}

		if (m_aSlots.IsEmpty())
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	static void ReadTransform(IEntity slotEntity, EL_EntitySaveData saveData, EL_EntitySlotPrefabInfo prefabInfo, out EL_EReadResult readResult)
	{
		EL_PersistenceComponentClass slotAttributes = EL_ComponentData<EL_PersistenceComponentClass>.Get(slotEntity);
		if (saveData.m_pTransformation.ReadFrom(slotEntity, slotAttributes.m_pSaveData))
		{
			if (saveData.m_pTransformation.m_vOrigin != EL_Const.VEC_INFINITY &&
				vector.Distance(saveData.m_pTransformation.m_vOrigin, prefabInfo.m_vOffset) > 0.001)
				readResult = EL_EReadResult.OK;

			if (saveData.m_pTransformation.m_vAngles != EL_Const.VEC_INFINITY)
			{
				vector localFixedAngles = slotEntity.GetLocalAngles();
				if (float.AlmostEqual(localFixedAngles[0], -180))
					localFixedAngles[0] = 180;

				if (float.AlmostEqual(localFixedAngles[1], -180))
					localFixedAngles[1] = 180;

				if (float.AlmostEqual(localFixedAngles[2], -180))
					localFixedAngles[2] = 180;

				if (vector.Distance(localFixedAngles, prefabInfo.m_vAngles) > 0.001)
					readResult = EL_EReadResult.OK;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		SlotManagerComponent slotManager = SlotManagerComponent.Cast(component);

		array<EntitySlotInfo> outSlotInfos();
		slotManager.GetSlotInfos(outSlotInfos);

		array<ref EL_EntitySlotPrefabInfo> slotinfos = EL_EntitySlotPrefabInfo.GetSlotInfos(owner, slotManager);

		EL_EApplyResult result = EL_EApplyResult.OK;

		// TODO: Refactor to use find by name etc once we have https://feedback.bistudio.com/T171679
		foreach (EL_PersistentEntitySlot slot : m_aSlots)
		{
			foreach (int idx, EL_EntitySlotPrefabInfo slotInfo : slotinfos)
			{
				if (slotInfo.m_sName == slot.m_sName)
				{
					EL_EApplyResult slotResult = ApplySlot(outSlotInfos.Get(idx), slot.m_pEntity);
					if (slotResult == EL_EApplyResult.ERROR)
						return EL_EApplyResult.ERROR;

					if (slotResult == EL_EApplyResult.AWAIT_COMPLETION)
						result = EL_EApplyResult.AWAIT_COMPLETION;
				}
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	static EL_EApplyResult ApplySlot(EntitySlotInfo entitySlot, EL_EntitySaveData saveData)
	{
		IEntity slotEntity = entitySlot.GetAttachedEntity();

		// If there is an tramsform override saved we need to consume it before load operations
		EL_PersistentTransformation persistentTransform;
		if (saveData)
		{
			persistentTransform = saveData.m_pTransformation;
			saveData.m_pTransformation = null;
		}

		// Found matching entity, no need to spawn, just apply save-data
		if (saveData &&
			slotEntity &&
			EL_Utils.GetPrefabName(slotEntity) == saveData.m_rPrefab)
		{
			EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
			if (slotPersistence && !slotPersistence.Load(saveData, false))
				return EL_EApplyResult.ERROR;
		}
		else
		{
			// Slot did not match save-data, delete current entity on it
			SCR_EntityHelper.DeleteEntityAndChildren(slotEntity);

			if (!saveData)
				return EL_EApplyResult.OK;

			// Spawn new entity and attach it
			slotEntity = saveData.Spawn(false);
			if (!slotEntity)
				return EL_EApplyResult.ERROR;

			entitySlot.AttachEntity(slotEntity);
			if (entitySlot.GetAttachedEntity() != slotEntity)
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

			entitySlot.OverrideTransformLS(transform);
		}

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_SlotManagerComponentSaveData otherData = EL_SlotManagerComponentSaveData.Cast(other);

		if (m_aSlots.Count() != otherData.m_aSlots.Count())
			return false;

		foreach (int idx, EL_PersistentEntitySlot slot : m_aSlots)
		{
			// Try same index first as they are likely to be the correct ones.
			if (slot.Equals(otherData.m_aSlots.Get(idx)))
				continue;

			bool found;
			foreach (int compareIdx, EL_PersistentEntitySlot otherSlot : otherData.m_aSlots)
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

class EL_PersistentEntitySlot
{
	string m_sName;
	ref EL_EntitySaveData m_pEntity;

	//------------------------------------------------------------------------------------------------
	bool Equals(notnull EL_PersistentEntitySlot other)
	{
		return m_sName == other.m_sName && m_pEntity.Equals(other.m_pEntity);
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid())
			return false;

		saveContext.WriteValue("m_sName", m_sName);

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

		loadContext.ReadValue("m_sName", m_sName);

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
