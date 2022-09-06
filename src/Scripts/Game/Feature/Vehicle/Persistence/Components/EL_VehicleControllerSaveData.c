[EL_ComponentSaveDataType(EL_VehicleControllerSaveData, VehicleControllerComponent, "VehicleController"), BaseContainerProps()]
class EL_VehicleControllerSaveData : EL_ComponentSaveDataBase
{
	bool m_bEngineOn;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(worldEntityComponent);
		m_bEngineOn = vehicleController.IsEngineOn();
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(worldEntityComponent);
		if (m_bEngineOn) vehicleController.StartEngine();
		return true;
	}
}
