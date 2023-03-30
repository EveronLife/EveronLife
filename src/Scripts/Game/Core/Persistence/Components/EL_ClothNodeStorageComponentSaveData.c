[EL_ComponentSaveDataType(EL_ClothNodeStorageComponentSaveDataClass, ClothNodeStorageComponent), BaseContainerProps()]
class EL_ClothNodeStorageComponentSaveDataClass : EL_BaseInventoryStorageComponentSaveDataClass
{
}

[EL_DbName(EL_ClothNodeStorageComponentSaveData, "ClothNodeStorage")]
class EL_ClothNodeStorageComponentSaveData : EL_BaseInventoryStorageComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		if (!super.ReadFrom(worldEntityComponent, attributes)) return false;

		// Override the storage slots as cloth storages handle this internally
		foreach (EL_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			slot.m_iSlotId = -1;
		}

		return true;
	}
}
