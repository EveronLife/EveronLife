[EL_ComponentSaveDataType(MuzzleInMagComponent), BaseContainerProps()]
class EL_MuzzleInMagComponentSaveDataClass : EL_BaseMuzzleComponentSaveDataClass
{
};

[EL_DbName.Automatic()]
class EL_MuzzleInMagComponentSaveData : EL_BaseMuzzleComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override protected bool IsDefaultChambered(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		MuzzleInMagComponent muzzleInMag = MuzzleInMagComponent.Cast(component);
		BaseContainer muzzleInMagSource = muzzleInMag.GetComponentSource(owner);

		int initialAmmo;
		muzzleInMagSource.Get("InitialAmmo", initialAmmo);

		ResourceName ammoTemplate;
		if (initialAmmo > 0)
			muzzleInMagSource.Get("AmmoTemplate", ammoTemplate);

		return ammoTemplate;
	}
};
