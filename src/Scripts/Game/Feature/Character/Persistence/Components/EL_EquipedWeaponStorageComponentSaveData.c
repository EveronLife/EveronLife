[EL_ComponentSaveDataType(EL_EquipedWeaponStorageComponentSaveDataClass, EquipedWeaponStorageComponent), BaseContainerProps()]
class EL_EquipedWeaponStorageComponentSaveDataClass : EL_BaseInventoryStorageComponentSaveDataClass
{
}

[EL_DbName(EL_EquipedWeaponStorageComponentSaveData, "EquipedWeaponStorage")]
class EL_EquipedWeaponStorageComponentSaveData : EL_BaseInventoryStorageComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		EquipedWeaponStorageComponent storageComponent = EquipedWeaponStorageComponent.Cast(worldEntityComponent);

		m_iPriority = storageComponent.GetPriority();
		m_ePurposeFlags = storageComponent.GetPurpose();

		m_aSlots = new array<ref EL_PersistentInventoryStorageSlot>();

		for (int i = 0, slots = storageComponent.GetSlotsCount(); i < slots; i++)
		{
			IEntity slotEntity = storageComponent.Get(i);
			if (!slotEntity) continue;

			EL_PersistenceComponent slotPersistenceComponent = EL_PersistenceComponent.Cast(slotEntity.FindComponent(EL_PersistenceComponent));
			if (!slotPersistenceComponent) continue;

			EL_EntitySaveData saveData = slotPersistenceComponent.Save();
			if (!saveData) continue;

			// Reset transformation data, as that won't be needed for stored entites
			saveData.m_pTransformation.Reset();

			EL_PersistentInventoryStorageSlot slotInfo();
			slotInfo.m_iSlotId = i;
			slotInfo.m_pEntity = saveData;
			m_aSlots.Insert(slotInfo);
		}

		return true;
	}
}
