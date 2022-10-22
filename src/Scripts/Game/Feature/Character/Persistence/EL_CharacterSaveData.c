[EL_DbName(EL_CharacterSaveData, "Character"), BaseContainerProps()]
class EL_CharacterSaveData : EL_EntitySaveDataBase
{
	[Attribute(defvalue: "30", uiwidget: UIWidgets.Slider, desc: "Maximum time until the quickbar is synced after a change in SECONDS. Higher values reduce traffic.", params: "1 1000 1"), NonSerialized()]
	int m_iMaxQuickbackSaveTime;

	ECharacterStance m_eStance;

	string m_sLeftHandItemId;
	string m_sRightHandItemId;
	EEquipItemType m_eRightHandType;
	bool m_bRightHandRaised;

	ref array<ref EL_PersistentQuickSlotItem> m_aQuickSlotEntities;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull IEntity worldEntity)
	{
		if (!super.ReadFrom(worldEntity)) return false;

		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(worldEntity.FindComponent(CharacterControllerComponent));
		if (characterController)
		{
			m_eStance = characterController.GetStance();

			m_sLeftHandItemId = EL_PersistenceComponent.GetPersistentId(characterController.GetAttachedGadgetAtLeftHandSlot());
			m_sRightHandItemId = EL_PersistenceComponent.GetPersistentId(characterController.GetRightHandItem());
			if (m_sRightHandItemId)
			{
				m_eRightHandType = EEquipItemType.EEquipTypeGeneric;
			}
			else
			{
				BaseWeaponManagerComponent weaponManager = characterController.GetWeaponManagerComponent();
				if (weaponManager && weaponManager.GetCurrentSlot())
				{
					m_sRightHandItemId = EL_PersistenceComponent.GetPersistentId(weaponManager.GetCurrentSlot().GetWeaponEntity());
					m_eRightHandType = EEquipItemType.EEquipTypeWeapon;
					m_bRightHandRaised = characterController.IsWeaponRaised();
				}
			}
		}

		m_aQuickSlotEntities = new array<ref EL_PersistentQuickSlotItem>();

		SCR_CharacterInventoryStorageComponent inventoryStorage = SCR_CharacterInventoryStorageComponent.Cast(worldEntity.FindComponent(SCR_CharacterInventoryStorageComponent));
		if (inventoryStorage)
		{
			foreach (int idx, IEntity item : inventoryStorage.GetQuickSlotItems())
			{
				string persistentId = EL_PersistenceComponent.GetPersistentId(item);
				if (persistentId)
				{
					EL_PersistentQuickSlotItem slot();
					slot.m_iIndex = idx;
					slot.m_sEntityId = persistentId;
					m_aQuickSlotEntities.Insert(slot);
				}
			}
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!super.SerializationSave(saveContext)) return false;

		saveContext.WriteValue("m_eStance", m_eStance);
		saveContext.WriteValue("m_sLeftHandItemId", m_sLeftHandItemId);
		saveContext.WriteValue("m_sRightHandItemId", m_sRightHandItemId);
		saveContext.WriteValue("m_eRightHandType", m_eRightHandType);
		saveContext.WriteValue("m_bRightHandRaised", m_bRightHandRaised);
		saveContext.WriteValue("m_aQuickSlotEntities", m_aQuickSlotEntities);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!super.SerializationLoad(loadContext)) return false;

		loadContext.ReadValue("m_eStance", m_eStance);
		loadContext.ReadValue("m_sLeftHandItemId", m_sLeftHandItemId);
		loadContext.ReadValue("m_sRightHandItemId", m_sRightHandItemId);
		loadContext.ReadValue("m_eRightHandType", m_eRightHandType);
		loadContext.ReadValue("m_bRightHandRaised", m_bRightHandRaised);
		loadContext.ReadValue("m_aQuickSlotEntities", m_aQuickSlotEntities);
		return true;
	}
}

class EL_PersistentQuickSlotItem
{
	int m_iIndex;
	string m_sEntityId;
}
