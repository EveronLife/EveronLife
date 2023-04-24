[EL_ComponentSaveDataType(EL_BaseMagazineComponentSaveDataClass, BaseMagazineComponent), BaseContainerProps()]
class EL_BaseMagazineComponentSaveDataClass : EL_ComponentSaveDataClass
{
}

[EL_DbName(EL_BaseMagazineComponentSaveData, "Magazine")]
class EL_BaseMagazineComponentSaveData : EL_ComponentSaveData
{
	int m_iAmmoCount;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseMagazineComponent magazine = BaseMagazineComponent.Cast(worldEntityComponent);
		m_iAmmoCount = magazine.GetAmmoCount();
		if (magazine.IsUsed()) m_iAmmoCount++; // Add one for when its loaded back because it consumes one for chambering

		if (m_iAmmoCount >= magazine.GetMaxAmmoCount()) return EL_EReadResult.DEFAULT;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseMagazineComponent magazine = BaseMagazineComponent.Cast(worldEntityComponent);
		magazine.SetAmmoCount(m_iAmmoCount);
		return true;
	}
}
