[EL_ComponentSaveDataType(EL_BaseLightManagerComponentSaveDataClass, BaseLightManagerComponent), BaseContainerProps()]
class EL_BaseLightManagerComponentSaveDataClass : EL_ComponentSaveDataClass
{
}

[EL_DbName(EL_BaseLightManagerComponentSaveData, "LightManager")]
class EL_BaseLightManagerComponentSaveData : EL_ComponentSaveData
{
	ref array<ref EL_PersistentLightSlot> m_aLightSlots;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseLightManagerComponent lightManager = BaseLightManagerComponent.Cast(worldEntityComponent);

		m_aLightSlots = new array<ref EL_PersistentLightSlot>();

		array<BaseLightSlot> lightSlots();
		lightManager.GetLights(lightSlots);
		foreach (BaseLightSlot lightSlot : lightSlots)
		{
			EL_PersistentLightSlot persistentLightSlot();
			persistentLightSlot.m_eType = lightSlot.GetLightType();
			persistentLightSlot.m_iSide = lightSlot.GetLightSide();
			persistentLightSlot.m_bFunctional = lightSlot.IsLightFunctional();
			persistentLightSlot.m_bState = lightManager.GetLightsState(persistentLightSlot.m_eType, persistentLightSlot.m_iSide);

			if (attributes.m_bTrimDefaults && persistentLightSlot.m_bFunctional && !persistentLightSlot.m_bState) continue;
			m_aLightSlots.Insert(persistentLightSlot);
		}

		if (m_aLightSlots.IsEmpty()) return EL_EReadResult.DEFAULT;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		BaseLightManagerComponent lightManager = BaseLightManagerComponent.Cast(worldEntityComponent);

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
				if (lightSlot.IsPresence()) persistentLightSlot.m_eType = ELightType.Presence;

				lightManager.SetLightsState(persistentLightSlot.m_eType, persistentLightSlot.m_bState, persistentLightSlot.m_iSide);
				lightSlots.Remove(idx);

				PrintFormat("Set %1 for side %2 to %3", typename.EnumToString(ELightType, persistentLightSlot.m_eType), persistentLightSlot.m_iSide, persistentLightSlot.m_bState);

				break;
			}
		}

		return true;
	}
}

class EL_PersistentLightSlot
{
	ELightType m_eType;
	int m_iSide;
	bool m_bFunctional;
	bool m_bState;
}
