[EL_ComponentSaveDataType(WeaponAttachmentsStorageComponent), BaseContainerProps()]
class EL_WeaponAttachmentsStorageComponentSaveDataClass : EL_BaseInventoryStorageComponentSaveDataClass
{
	override array<typename> Requires()
	{
		return {EL_BaseMuzzleComponentSaveDataClass};
	}
};

class EL_WeaponAttachmentsStorageComponentSaveData : EL_BaseInventoryStorageComponentSaveData
{
};
