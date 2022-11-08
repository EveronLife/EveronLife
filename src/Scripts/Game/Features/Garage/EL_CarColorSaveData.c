[EL_ComponentSaveDataType(EL_CarColorSaveData, ParametricMaterialInstanceComponent, "CarColor"), BaseContainerProps()]
class EL_CarColorSaveData : EL_ComponentSaveDataBase
{
	int m_iVehicleColor;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		ParametricMaterialInstanceComponent carMaterialComponent = ParametricMaterialInstanceComponent.Cast(worldEntityComponent);
		m_iVehicleColor = carMaterialComponent.GetColor();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		ParametricMaterialInstanceComponent carMaterialComponent = ParametricMaterialInstanceComponent.Cast(worldEntityComponent);
		carMaterialComponent.SetColor(m_iVehicleColor);

		return true;
	}
}
