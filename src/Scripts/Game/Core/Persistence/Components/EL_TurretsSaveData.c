/*
[EL_ComponentSaveDataType(SlotManagerComponent), BaseContainerProps()]
class EL_TurretsSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName("Turrets")]
class EL_TurretsSaveData : EL_ComponentSaveData
{
	ref array<ref EL_PersistentTurretState> m_aTurretStates;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		SlotManagerComponent slotManager = SlotManagerComponent.Cast(component);

		m_aTurretStates = {};

		array<string> slotNames = EL_SlotsManagerHelper.GetSlotNames(owner, slotManager);

		array<EntitySlotInfo> outSlotInfos();
		slotManager.GetSlotInfos(outSlotInfos);
		foreach (int idx, EntitySlotInfo slotInfo : outSlotInfos)
		{
			TurretComponent turret = EL_Component<TurretComponent>.Find(slotInfo.GetAttachedEntity());
			if (!turret)
				continue;

			vector angles = turret.GetAimingRotation();
			
			EL_PersistentTurretState turretState();
			turretState.m_sSlotName = slotNames.Get(idx);
			turretState.m_fYaw = angles[0];
			turretState.m_fPitch = angles[1];

			if (attributes.m_bTrimDefaults && float.AlmostEqual(turretState.m_fYaw, 0.0) && float.AlmostEqual(turretState.m_fPitch, 0.0))
				continue;
			
			m_aTurretStates.Insert(turretState);
		}

		if (m_aTurretStates.IsEmpty())
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		SlotManagerComponent slotManager = SlotManagerComponent.Cast(component);

		array<EntitySlotInfo> outSlotInfos();
		slotManager.GetSlotInfos(outSlotInfos);

		array<string> slotNames = EL_SlotsManagerHelper.GetSlotNames(owner, slotManager);
		
		foreach (EL_PersistentTurretState turretState : m_aTurretStates)
		{
			int idx = slotNames.Find(turretState.m_sSlotName);
			if (idx == -1)
				continue;

			EntitySlotInfo slotInfo = outSlotInfos.Get(idx);
			if (!slotInfo)
				continue;

			TurretControllerComponent turretController = EL_Component<TurretControllerComponent>.Find(slotInfo.GetAttachedEntity());
			if (!turretController)
				continue;

			// Convert to radians because somebody decided the getter should be degrees, even though setter is radians :)
			turretController.SetAimingAngles(turretState.m_fYaw * Math.DEG2RAD, turretState.m_fPitch * Math.DEG2RAD);
		}

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_TurretsSaveData otherData = EL_TurretsSaveData.Cast(other);

		if (m_aTurretStates.Count() != otherData.m_aTurretStates.Count())
			return false;

		foreach (int idx, EL_PersistentTurretState turretState : m_aTurretStates)
		{
			// Try same index first as they are likely to be the correct ones.
			if (turretState.Equals(otherData.m_aTurretStates.Get(idx)))
				continue;

			bool found;
			foreach (int compareIdx, EL_PersistentTurretState otherTurretState : otherData.m_aTurretStates)
			{
				if (compareIdx == idx)
					continue; // Already tried in idx direct compare

				if (turretState.Equals(otherTurretState))
				{
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}

		return true;
	}
};

class EL_PersistentTurretState
{
	string m_sSlotName;
	float m_fYaw;
	float m_fPitch;

	//------------------------------------------------------------------------------------------------
	bool Equals(notnull EL_PersistentTurretState other)
	{
		return m_sSlotName == other.m_sSlotName &&
			float.AlmostEqual(m_fYaw, other.m_fYaw) &&
			float.AlmostEqual(m_fPitch, other.m_fPitch);
	}
};
*/