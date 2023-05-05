[EL_ComponentSaveDataType(TurretControllerComponent), BaseContainerProps()]
class EL_TurretControllerComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName("TurretController")]
class EL_TurretControllerComponentSaveData : EL_ComponentSaveData
{
	float m_fYaw;
	float m_fPitch;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		TurretControllerComponent turretController = TurretControllerComponent.Cast(component);

		TurretComponent turret = turretController.GetTurretComponent();
		if (!turret)
			return EL_EReadResult.ERROR;

		vector angles = turret.GetAimingRotation();
		m_fYaw = angles[0];
		m_fPitch = angles[1];

		vector initAiming;

		// TODO: Remove this workaround once https://feedback.bistudio.com/T172138 is added
		array<Managed> outComponents();
		owner.FindComponents(WeaponSlotComponent, outComponents);
		foreach (Managed componentRef : outComponents)
		{
			WeaponSlotComponent weaponSlot = WeaponSlotComponent.Cast(componentRef);
			IEntity weaponEntity = weaponSlot.GetWeaponEntity();
			TurretComponent weaponTurret = EL_Component<TurretComponent>.Find(weaponEntity);

			if (weaponTurret == turret)
			{
				BaseContainer turretSource = weaponTurret.GetComponentSource(weaponEntity);
				turretSource.Get("InitAiming", initAiming);
			}
		}

		// Check if there is any diff to default angles
		if (vector.Distance(angles, initAiming) <= 0.0001)
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		TurretControllerComponent turretController = TurretControllerComponent.Cast(component);
		turretController.SetAimingAngles(m_fYaw * Math.DEG2RAD, m_fPitch * Math.DEG2RAD);
		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_TurretControllerComponentSaveData otherData = EL_TurretControllerComponentSaveData.Cast(other);
		return float.AlmostEqual(m_fYaw, otherData.m_fYaw) && float.AlmostEqual(m_fPitch, otherData.m_fPitch);
	}
};
