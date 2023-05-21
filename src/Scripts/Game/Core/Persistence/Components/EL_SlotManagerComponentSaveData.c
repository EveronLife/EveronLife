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
		array<ref Tuple2<string, ResourceName>> slotinfos = EL_EntitySlotPrefabInfo.GetSlotInfos(owner, slotManager);

		array<EntitySlotInfo> outSlotInfos();
		slotManager.GetSlotInfos(outSlotInfos);
		foreach (int idx, EntitySlotInfo entitySlot : outSlotInfos)
		{
			IEntity slotEntity = entitySlot.GetAttachedEntity();
			ResourceName prefab = EL_Utils.GetPrefabName(slotEntity);

			Tuple2<string, ResourceName> prefabInfo = slotinfos.Get(idx);
			bool isPrefabMatch = prefab == prefabInfo.param2;

			EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
			if (!slotPersistence)
			{
				// Slot is different from prefab and the new slot has no persistence component so we clear it.
				if (!isPrefabMatch)
				{
					EL_PersistentEntitySlot persistentSlot();
					persistentSlot.m_sName = prefabInfo.param1;
					m_aSlots.Insert(persistentSlot);
				}

				continue;
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
				continue;
			}

			EL_PersistentEntitySlot persistentSlot();
			persistentSlot.m_sName = prefabInfo.param1;
			persistentSlot.m_pEntity = saveData;
			m_aSlots.Insert(persistentSlot);
		}

		if (m_aSlots.IsEmpty())
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		SlotManagerComponent slotManager = SlotManagerComponent.Cast(component);

		array<EntitySlotInfo> outSlotInfos();
		slotManager.GetSlotInfos(outSlotInfos);

		array<ref Tuple2<string, ResourceName>> slotinfos = EL_EntitySlotPrefabInfo.GetSlotInfos(owner, slotManager);

		// TODO: Refactor to use find by name etc once we have https://feedback.bistudio.com/T171679
		foreach (EL_PersistentEntitySlot slot : m_aSlots)
		{
			foreach (int idx, Tuple2<string, ResourceName> slotInfo : slotinfos)
			{
				if (slotInfo.param1 == slot.m_sName)
				{
					EntitySlotInfo entitySlot = outSlotInfos.Get(idx);
					IEntity slotEntity = entitySlot.GetAttachedEntity();

					// If there is an tramsform override saved we need to consume it before load operations
					EL_PersistentTransformation persistentTransform;
					if (slot.m_pEntity)
					{
						persistentTransform = slot.m_pEntity.m_pTransformation;
						slot.m_pEntity.m_pTransformation = null;
					}

					// Found matching entity, no need to spawn, just apply save-data
					if (slot.m_pEntity &&
						slotEntity &&
						EL_Utils.GetPrefabName(slotEntity) == slot.m_pEntity.m_rPrefab)
					{
						EL_PersistenceComponent slotPersistence = EL_Component<EL_PersistenceComponent>.Find(slotEntity);
						if (slotPersistence && !slotPersistence.Load(slot.m_pEntity, false))
							return EL_EApplyResult.ERROR;
					}
					else
					{
						// Slot did not match save-data, delete current entity on it
						SCR_EntityHelper.DeleteEntityAndChildren(slotEntity);

						if (!slot.m_pEntity)
							continue;

						// Spawn new entity and attach it
						slotEntity = slot.m_pEntity.Spawn(false);
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
			}
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
