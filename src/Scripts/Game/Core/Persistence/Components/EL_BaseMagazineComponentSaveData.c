[EL_ComponentSaveDataType(BaseMagazineComponent), BaseContainerProps()]
class EL_BaseMagazineComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName("Magazine")]
class EL_BaseMagazineComponentSaveData : EL_ComponentSaveData
{
	int m_iAmmoCount;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseMagazineComponent magazine = BaseMagazineComponent.Cast(component);
		m_iAmmoCount = magazine.GetAmmoCount();

		if (m_iAmmoCount >= magazine.GetMaxAmmoCount())
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseMagazineComponent magazine = BaseMagazineComponent.Cast(component);
		magazine.SetAmmoCount(m_iAmmoCount);
		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_BaseMagazineComponentSaveData otherData = EL_BaseMagazineComponentSaveData.Cast(other);
		return m_iAmmoCount == otherData.m_iAmmoCount;
	}
};
