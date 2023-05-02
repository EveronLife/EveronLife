[EL_ComponentSaveDataType(WeaponAttachmentsStorageComponent), BaseContainerProps()]
class EL_WeaponAttachmentsStorageComponentSaveDataClass : EL_BaseInventoryStorageComponentSaveDataClass
{
	override array<typename> Requires()
	{
		return {EL_BaseMuzzleComponentSaveDataClass};
	}
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
				BaseContainer magazinePrefab = muzzle.GetDefaultMagazinePrefab();
				if (magazinePrefab)
					result.Insert(magazinePrefab.GetResourceName());
			}
		}
		return result;
	}
};
