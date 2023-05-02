[EL_ComponentSaveDataType(WeaponAttachmentsStorageComponent), BaseContainerProps()]
class EL_WeaponAttachmentsStorageComponentSaveDataClass : EL_BaseInventoryStorageComponentSaveDataClass
{
};

[EL_DbName("WeaponAttachmentsStorage")]
class EL_WeaponAttachmentsStorageComponentSaveData : EL_BaseInventoryStorageComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override protected array<string> GetDefaultPrefabs(BaseInventoryStorageComponent storageComponent)
	{
		array<string> result = EL_PersistencePrefabInfo.GetPrefabChildren(storageComponent);
		if (result)
		{
			MuzzleComponent muzzle = EL_Component<MuzzleComponent>.Find(storageComponent.GetOwner());
			if (muzzle)
			{
				ResourceName magazinePrefab = muzzle.GetDefaultMagazinePrefab().GetResourceName();
				if (magazinePrefab)
					result.Insert(magazinePrefab);
			}
		}
		return result;
	}
};
