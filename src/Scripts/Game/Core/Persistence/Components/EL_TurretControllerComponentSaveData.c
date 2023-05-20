[EL_ComponentSaveDataType(TurretControllerComponent), BaseContainerProps()]
class EL_TurretControllerComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

class EL_TurretControllerComponentSaveData : EL_ComponentSaveData
{
	float m_fYaw;
	float m_fPitch;
	int m_iSelectedWeaponSlotIdx;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		TurretControllerComponent turretController = TurretControllerComponent.Cast(component);

		BaseWeaponManagerComponent weaponManager = turretController.GetWeaponManager();
		WeaponSlotComponent currentSlot = weaponManager.GetCurrentSlot();
		if (currentSlot)
			m_iSelectedWeaponSlotIdx = currentSlot.GetWeaponSlotIndex();

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

		int defaultIdx;
		BaseContainer weaponManagerSource = weaponManager.GetComponentSource(owner);
		weaponManagerSource.Get("DefaultWeaponIndex", defaultIdx);
		if (defaultIdx == -1)
		{
			WeaponSlotComponent firstSlot = EL_Component<WeaponSlotComponent>.Find(owner);
			if (firstSlot)
			{
				defaultIdx = firstSlot.GetWeaponSlotIndex();
			}
			else
			{
				defaultIdx = 0;
			}
		}

		if (vector.Distance(angles, initAiming) <= 0.0001 && (m_iSelectedWeaponSlotIdx == defaultIdx))
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		TurretControllerComponent turretController = TurretControllerComponent.Cast(component);
		turretController.SetAimingAngles(m_fYaw * Math.DEG2RAD, m_fPitch * Math.DEG2RAD);

		/*
		TODO: Wait for https://feedback.bistudio.com/T172197 to be added so we can select the weapon from the manager

		BaseWeaponManagerComponent weaponManager = turretController.GetWeaponManager();
		WeaponSlotComponent currentSlot = weaponManager.GetCurrentSlot();
		if (!currentSlot || currentSlot.GetWeaponSlotIndex() != m_iSelectedWeaponSlotIdx)
		{
			array<WeaponSlotComponent> outSlots();
			turretController.GetWeaponManager().GetWeaponsSlots(outSlots);
			foreach (WeaponSlotComponent weaponSlot : outSlots)
			{
				if (weaponSlot.GetWeaponSlotIndex() == m_iSelectedWeaponSlotIdx)
				{
					weaponManager.SelectWeapon(weaponSlot);
					break;
				}
			}
		}
		*/

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_TurretControllerComponentSaveData otherData = EL_TurretControllerComponentSaveData.Cast(other);
		return float.AlmostEqual(m_fYaw, otherData.m_fYaw) &&
			float.AlmostEqual(m_fPitch, otherData.m_fPitch) &&
			m_iSelectedWeaponSlotIdx == otherData.m_iSelectedWeaponSlotIdx;
	}
};
