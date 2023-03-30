[EL_ComponentSaveDataType(EL_HitZoneContainerComponentSaveDataClass, HitZoneContainerComponent), BaseContainerProps()]
class EL_HitZoneContainerComponentSaveDataClass : EL_ComponentSaveDataClass
{
	[Attribute(defvalue: "1", desc: "Persist only damaged hitzones to minimize save sizes. Can be combined with selection filter.")]
	bool m_bDamagedOnly;

	[Attribute(desc: "If any are provided, each hitzone must be explictly selected to be persisted.")]
	ref array<string> m_aHitzoneFilter;
}

[EL_DbName(EL_HitZoneContainerComponentSaveData, "HitZoneContainer")]
class EL_HitZoneContainerComponentSaveData : EL_ComponentSaveData
{
	ref array<ref EL_PersistentHitZone> m_aHitzones;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		EL_HitZoneContainerComponentSaveDataClass settings = EL_HitZoneContainerComponentSaveDataClass.Cast(attributes);
		HitZoneContainerComponent hitZoneContainer = HitZoneContainerComponent.Cast(worldEntityComponent);

		m_aHitzones = new array<ref EL_PersistentHitZone>();

		array<HitZone> outHitZones();
		hitZoneContainer.GetAllHitZones(outHitZones);

		foreach (HitZone hitZone : outHitZones)
		{
			float healthScaled = hitZone.GetHealthScaled();
			if (settings.m_bDamagedOnly && float.AlmostEqual(healthScaled, 1.0)) continue;

			EL_PersistentHitZone persistentHitZone();
			persistentHitZone.m_sName = hitZone.GetName();
			persistentHitZone.m_fHealth = healthScaled;

			if (settings.m_aHitzoneFilter.IsEmpty() || settings.m_aHitzoneFilter.Contains(persistentHitZone.m_sName))
			{
				m_aHitzones.Insert(persistentHitZone);
			}
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		array<HitZone> outHitZones();
		HitZoneContainerComponent.Cast(worldEntityComponent).GetAllHitZones(outHitZones);

		bool tryIdxAcces = outHitZones.Count() >= m_aHitzones.Count();

		foreach (int idx, EL_PersistentHitZone persistentHitZone : m_aHitzones)
		{
			HitZone hitZone;

			// Assume same ordering as on save and see if that matches
			if (tryIdxAcces)
			{
				HitZone idxHitZone = outHitZones.Get(idx);

				if (idxHitZone.GetName() == persistentHitZone.m_sName) hitZone = idxHitZone;
			}

			// Iterate all hitzones to hopefully find the right one
			if (!hitZone)
			{
				foreach (HitZone findHitZone : outHitZones)
				{
					if (findHitZone.GetName() == persistentHitZone.m_sName)
					{
						hitZone = findHitZone;
						break;
					}
				}
			}

			if (!hitZone)
			{
				Debug.Error(string.Format("'%1' unable to find hitZone with name '%2'. Ignored.", worldEntityComponent, persistentHitZone.m_sName));
				continue;
			}

			hitZone.SetHealthScaled(persistentHitZone.m_fHealth);
		}

		return true;
	}
}

class EL_PersistentHitZone
{
	string m_sName;
	float m_fHealth;
}
