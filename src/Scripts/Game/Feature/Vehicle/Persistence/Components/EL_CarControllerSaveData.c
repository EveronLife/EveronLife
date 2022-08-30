[EL_ComponentSaveDataType(EL_CarControllerSaveData, CarControllerComponent, "CarController"), BaseContainerProps()]
class EL_CarControllerSaveData : EL_VehicleControllerSaveData
{
	bool m_bHandBrake;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		if (!super.ReadFrom(worldEntityComponent)) return false;

		CarControllerComponent carController = CarControllerComponent.Cast(worldEntityComponent);
		m_bHandBrake = carController.GetPersistentHandBrake();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		if (!super.ApplyTo(worldEntityComponent)) return false;

		CarControllerComponent carController = CarControllerComponent.Cast(worldEntityComponent);
		carController.SetPersistentHandBrake(m_bHandBrake);

		return true;
	}
}
