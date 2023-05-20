[EL_ComponentSaveDataType(BaseLightManagerComponent), BaseContainerProps()]
class EL_BaseLightManagerComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

class EL_BaseLightManagerComponentSaveData : EL_ComponentSaveData
{
	ref array<ref EL_PersistentLightSlot> m_aLightSlots;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseLightManagerComponent lightManager = BaseLightManagerComponent.Cast(component);

		m_aLightSlots = {};

		array<BaseLightSlot> lightSlots();
		lightManager.GetLights(lightSlots);
		foreach (BaseLightSlot lightSlot : lightSlots)
		{
			EL_PersistentLightSlot persistentLightSlot();
			persistentLightSlot.m_eType = lightSlot.GetLightType();
			persistentLightSlot.m_iSide = lightSlot.GetLightSide();
			persistentLightSlot.m_bFunctional = lightSlot.IsLightFunctional();
			persistentLightSlot.m_bState = lightManager.GetLightsState(persistentLightSlot.m_eType, persistentLightSlot.m_iSide);

			if (attributes.m_bTrimDefaults && persistentLightSlot.m_bFunctional && !persistentLightSlot.m_bState)
				continue;

			m_aLightSlots.Insert(persistentLightSlot);
		}

		if (m_aLightSlots.IsEmpty())
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseLightManagerComponent lightManager = BaseLightManagerComponent.Cast(component);

		array<BaseLightSlot> lightSlots();
		lightManager.GetLights(lightSlots);
		foreach (EL_PersistentLightSlot persistentLightSlot : m_aLightSlots)
		{
			foreach (int idx, BaseLightSlot lightSlot : lightSlots)
			{
				if (lightSlot.GetLightType() != persistentLightSlot.m_eType ||
					lightSlot.GetLightSide() != persistentLightSlot.m_iSide) continue;

				lightSlot.SetLightFunctional(persistentLightSlot.m_bFunctional);

				// TODO: Remove this hacky fix after https://feedback.bistudio.com/T171832 has been adressed
				ELightType lightType = persistentLightSlot.m_eType;
				if (lightSlot.IsPresence())
					lightType = ELightType.Presence;

				lightManager.SetLightsState(lightType, persistentLightSlot.m_bState, persistentLightSlot.m_iSide);
				lightSlots.Remove(idx);

				break;
			}
		}

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_BaseLightManagerComponentSaveData otherData = EL_BaseLightManagerComponentSaveData.Cast(other);

		if (m_aLightSlots.Count() != otherData.m_aLightSlots.Count())
			return false;

		foreach (int idx, EL_PersistentLightSlot lightSlot : m_aLightSlots)
		{
			// Try same index first as they are likely to be the correct ones.
			if (lightSlot.Equals(otherData.m_aLightSlots.Get(idx)))
				continue;

			bool found;
			foreach (int compareIdx, EL_PersistentLightSlot otherLightSlot : otherData.m_aLightSlots)
			{
				if (compareIdx == idx)
					continue; // Already tried in idx direct compare

				if (lightSlot.Equals(otherLightSlot))
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

class EL_PersistentLightSlot
{
	ELightType m_eType;
	int m_iSide;
	bool m_bFunctional;
	bool m_bState;

	//------------------------------------------------------------------------------------------------
	bool Equals(notnull EL_PersistentLightSlot other)
	{
		return m_eType == other.m_eType &&
			m_iSide == other.m_iSide &&
			m_bFunctional == other.m_bFunctional &&
			m_bState == other.m_bState;
	}
};
