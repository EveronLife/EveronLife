[EL_ComponentSaveDataType(VehicleControllerComponent), BaseContainerProps()]
class EL_VehicleControllerSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName.Automatic()]
class EL_VehicleControllerSaveData : EL_ComponentSaveData
{
	bool m_bEngineOn;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(component);
		m_bEngineOn = vehicleController.IsEngineOn();

		if (!m_bEngineOn)
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(component);

		if (m_bEngineOn)
			vehicleController.StartEngine();

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_VehicleControllerSaveData otherData = EL_VehicleControllerSaveData.Cast(other);
		return m_bEngineOn == otherData.m_bEngineOn;
	}
};
