[EL_ComponentSaveDataType(EL_ClothNodeStorageComponentSaveData, ClothNodeStorageComponent, "ClothNodeStorage"), BaseContainerProps()]
class EL_ClothNodeStorageComponentSaveData : EL_BaseInventoryStorageComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		if (!super.ReadFrom(worldEntityComponent)) return false;

		// Override the storage slots as cloth storages handle this internally
		foreach (EL_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			slot.m_iSlotId = -1;
		}

		return true;
	}
}
