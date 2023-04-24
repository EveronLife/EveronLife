[EL_ComponentSaveDataType(EL_VehicleControllerSaveDataClass, VehicleControllerComponent), BaseContainerProps()]
class EL_VehicleControllerSaveDataClass : EL_ComponentSaveDataClass
{
}

[EL_DbName(EL_VehicleControllerSaveData, "VehicleController")]
class EL_VehicleControllerSaveData : EL_ComponentSaveData
{
	bool m_bEngineOn;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(worldEntityComponent);
		m_bEngineOn = vehicleController.IsEngineOn();
		if (!m_bEngineOn) return EL_EReadResult.DEFAULT;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(worldEntityComponent);
		if (m_bEngineOn) vehicleController.StartEngine();
		return true;
	}
}
