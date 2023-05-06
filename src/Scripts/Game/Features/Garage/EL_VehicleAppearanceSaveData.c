[EL_ComponentSaveDataType(EL_VehicleAppearanceSaveData, EL_VehicleAppearanceComponent, "VehicleAppearance"), BaseContainerProps()]
class EL_VehicleAppearanceSaveData : EL_ComponentSaveDataBase
{
	protected int m_iVehicleColor = -1;
	protected ResourceName m_VehicleTexture;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		EL_VehicleAppearanceComponent vehicleAppearance = EL_VehicleAppearanceComponent.Cast(worldEntityComponent);
		m_iVehicleColor = vehicleAppearance.GetVehicleColor();
		m_VehicleTexture = vehicleAppearance.GetVehicleTexture();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		EL_VehicleAppearanceComponent vehicleAppearance = EL_VehicleAppearanceComponent.Cast(worldEntityComponent);
		vehicleAppearance.SetVehicleColor(m_iVehicleColor);
		vehicleAppearance.SetVehicleTexture(m_VehicleTexture);

		return true;
	}
}
