[EL_ComponentSaveDataType(EL_TransformationSaveDataClass, EL_TransformationSaveData), BaseContainerProps()]
class EL_TransformationSaveDataClass : EL_ComponentSaveDataClass
{
}

[EL_DbName(EL_TransformationSaveData, "Transformation")]
class EL_TransformationSaveData : EL_ComponentSaveData
{
	vector	m_vOrigin;
	vector	m_vAngles;
	float	m_fScale;

	//------------------------------------------------------------------------------------------------
	bool ReadFrom(IEntity worldEntity, notnull EL_ComponentSaveDataClass attributes)
	{
		m_vOrigin	= worldEntity.GetOrigin();
		m_vAngles	= worldEntity.GetLocalYawPitchRoll();
		m_fScale	= worldEntity.GetScale();
		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool ApplyTo(IEntity worldEntity, notnull EL_ComponentSaveDataClass attributes)
	{
		EL_Utils.Teleport(worldEntity, m_vOrigin, m_vAngles, m_fScale);
		return true;
	}
}
