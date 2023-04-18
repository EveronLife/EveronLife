[EL_ComponentSaveDataType(EL_CarControllerSaveDataClass, CarControllerComponent), BaseContainerProps()]
class EL_CarControllerSaveDataClass : EL_VehicleControllerSaveDataClass
{
}

[EL_DbName(EL_CarControllerSaveData, "CarController")]
class EL_CarControllerSaveData : EL_VehicleControllerSaveData
{
	bool m_bHandBrake;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		if (!super.ReadFrom(worldEntityComponent, attributes)) return false;

		CarControllerComponent carController = CarControllerComponent.Cast(worldEntityComponent);
		m_bHandBrake = carController.GetPersistentHandBrake();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		if (!super.ApplyTo(worldEntityComponent, attributes)) return false;

		CarControllerComponent carController = CarControllerComponent.Cast(worldEntityComponent);
		carController.SetPersistentHandBrake(m_bHandBrake);

		return true;
	}
}
