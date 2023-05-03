[EL_ComponentSaveDataType(MuzzleComponent), BaseContainerProps()]
class EL_MuzzleComponentSaveDataClass : EL_BaseMuzzleComponentSaveDataClass
{
};

[EL_DbName("Muzzle")]
class EL_MuzzleComponentSaveData : EL_BaseMuzzleComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		MuzzleComponent muzzle = MuzzleComponent.Cast(component);
		BaseContainer defaultMag = muzzle.GetDefaultMagazinePrefab();

		if (!super.ReadFrom(owner, component, attributes))
			return EL_EReadResult.ERROR;

		// Default mag attached so all chambers should be loaded
		if (defaultMag && !m_aChamberStatus.Contains(false))
			return EL_EReadResult.DEFAULT;

		// No default so all chambers should be empty
		if (!defaultMag && !m_aChamberStatus.Contains(true))
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}
};
