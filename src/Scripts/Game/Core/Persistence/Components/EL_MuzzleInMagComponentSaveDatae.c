[EL_ComponentSaveDataType(MuzzleInMagComponent), BaseContainerProps()]
class EL_MuzzleInMagComponentSaveDataClass : EL_BaseMuzzleComponentSaveDataClass
{
};

[EL_DbName("MuzzleInMag")]
class EL_MuzzleInMagComponentSaveData : EL_BaseMuzzleComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		if (!super.ReadFrom(worldEntityComponent, attributes))
			return EL_EReadResult.ERROR;

		// Assume all muzzle in mags have a default ammo chambered
		if (!m_aChamberStatus.Contains(false))
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}
};
