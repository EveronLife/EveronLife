[EL_ComponentSaveDataType(EL_BaseMagazineComponentSaveDataClass, BaseMagazineComponent), BaseContainerProps()]
class EL_BaseMagazineComponentSaveDataClass : EL_ComponentSaveDataClass
{
}

[EL_DbName(EL_BaseMagazineComponentSaveData, "Magazine")]
class EL_BaseMagazineComponentSaveData : EL_ComponentSaveData
{
	int m_iAmmoCount;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseMagazineComponent magazine = BaseMagazineComponent.Cast(worldEntityComponent);
		m_iAmmoCount = magazine.GetAmmoCount();
		if (magazine.IsUsed()) m_iAmmoCount++; // Add one for when its loaded back because it consumes one for chambering
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseMagazineComponent magazine = BaseMagazineComponent.Cast(worldEntityComponent);
		magazine.SetAmmoCount(m_iAmmoCount);
		return true;
	}
}
