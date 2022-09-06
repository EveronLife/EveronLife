[EL_ComponentSaveDataType(EL_BaseLightManagerComponentSaveData, BaseLightManagerComponent, "LightManager"), BaseContainerProps()]
class EL_BaseLightManagerComponentSaveData : EL_ComponentSaveDataBase
{
	ref array<ref EL_PersistentLightSlot> m_aLightSlots;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
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
			m_aLightSlots.Insert(persistentLightSlot);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		BaseLightManagerComponent lightManager = BaseLightManagerComponent.Cast(worldEntityComponent);

		array<BaseLightSlot> lightSlots();
		lightManager.GetLights(lightSlots);
		foreach (BaseLightSlot lightSlot : lightSlots)
		{
			foreach (EL_PersistentLightSlot persistentLightSlot : m_aLightSlots)
			{
				if (lightSlot.GetLightType() == persistentLightSlot.m_eType &&
					lightSlot.GetLightSide() == persistentLightSlot.m_iSide)
				{
					lightSlot.SetLightFunctional(persistentLightSlot.m_bFunctional);
					lightManager.SetLightsState(persistentLightSlot.m_eType, persistentLightSlot.m_bState, persistentLightSlot.m_iSide);
					break;
				}
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
