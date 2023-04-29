[EL_ComponentSaveDataType(EL_CarControllerSaveDataClass, CarControllerComponent), BaseContainerProps()]
class EL_CarControllerSaveDataClass : EL_VehicleControllerSaveDataClass
{
};

[EL_DbName(EL_CarControllerSaveData, "CarController")]
class EL_CarControllerSaveData : EL_VehicleControllerSaveData
{
	bool m_bHandBrake;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		EL_EReadResult readResult = super.ReadFrom(worldEntityComponent, attributes);
		if (!readResult) return EL_EReadResult.ERROR;

		CarControllerComponent carController = CarControllerComponent.Cast(worldEntityComponent);
		m_bHandBrake = carController.GetPersistentHandBrake();

		if (readResult == EL_EReadResult.DEFAULT && m_bHandBrake) return EL_EReadResult.DEFAULT;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		if (!super.ApplyTo(worldEntityComponent, attributes)) return false;

		CarControllerComponent carController = CarControllerComponent.Cast(worldEntityComponent);
		carController.SetPersistentHandBrake(m_bHandBrake);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_CarControllerSaveData otherData = EL_CarControllerSaveData.Cast(other);
		return super.Equals(other) && m_bHandBrake == otherData.m_bHandBrake;
	}
};
