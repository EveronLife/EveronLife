[EL_ComponentSaveDataType(EL_TransformationSaveData, EL_TransformationSaveData, "Transformation"), BaseContainerProps()]
class EL_TransformationSaveData : EL_ComponentSaveDataBase
{
	vector	m_vOrigin;
	vector	m_vAngles;
	float	m_fScale;

	//------------------------------------------------------------------------------------------------
	bool ReadFrom(IEntity worldEntity)
	{
		m_vOrigin	= worldEntity.GetOrigin();
		m_vAngles	= worldEntity.GetLocalYawPitchRoll();
		m_fScale	= worldEntity.GetScale();
		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool ApplyTo(IEntity worldEntity)
	{
		EL_Utils.Teleport(worldEntity, m_vOrigin, m_vAngles, m_fScale);
		return true;
	}
}
