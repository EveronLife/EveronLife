[EL_ComponentSaveDataType(EL_EquipedWeaponStorageComponentSaveData, EquipedWeaponStorageComponent, "EquipedWeaponStorage"), BaseContainerProps()]
class EL_EquipedWeaponStorageComponentSaveData : EL_BaseInventoryStorageComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
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

			EL_EntitySaveDataBase saveData = slotPersistenceComponent.Save();
			if (!saveData) continue;

			// Remove transformation data, as that won't be needed for stored entites
			saveData.m_mComponentsSaveData.Remove(EL_TransformationSaveData);

			EL_PersistentInventoryStorageSlot slotInfo();
			slotInfo.m_iSlotId = i;
			slotInfo.m_pEntity = saveData;
			m_aSlots.Insert(slotInfo)
		}

		return true;
	}
}
