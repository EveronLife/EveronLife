[EL_ComponentSaveDataType(EL_TransformationSaveData, EL_TransformationSaveData, "Transformation"), BaseContainerProps()]
class EL_TransformationSaveData : EL_ComponentSaveDataBase
{
	vector m_vOrigin;
	vector m_vAngles;
	
	bool ReadFrom(IEntity worldEntity)
	{
		m_vOrigin = worldEntity.GetOrigin();
		m_vAngles = worldEntity.GetLocalYawPitchRoll();
		return true;
	}
	
	bool ApplyTo(IEntity worldEntity)
	{
		vector transform[4];
		Math3D.AnglesToMatrix(m_vAngles, transform);
		transform[3] = m_vOrigin;
		
		BaseGameEntity baseGameEntity = BaseGameEntity.Cast(worldEntity);
		if (baseGameEntity)
		{
			baseGameEntity.Teleport(transform);
			return true;
		}
		
		worldEntity.SetWorldTransform(transform);
		return true;
	}
}
