[EL_ComponentSaveDataType(HitZoneContainerComponent), BaseContainerProps()]
class EL_HitZoneContainerComponentSaveDataClass : EL_ComponentSaveDataClass
{
	[Attribute(desc: "If set, only the explictly selected hitzones are persisted.")]
	ref array<string> m_aHitzoneFilter;
};

[EL_DbName("HitZoneContainer")]
class EL_HitZoneContainerComponentSaveData : EL_ComponentSaveData
{
	ref array<ref EL_PersistentHitZone> m_aHitzones;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		EL_HitZoneContainerComponentSaveDataClass settings = EL_HitZoneContainerComponentSaveDataClass.Cast(attributes);
		HitZoneContainerComponent hitZoneContainer = HitZoneContainerComponent.Cast(worldEntityComponent);

		m_aHitzones = {};

		array<HitZone> outHitZones();
		hitZoneContainer.GetAllHitZones(outHitZones);

		foreach (HitZone hitZone : outHitZones)
		{
			EL_PersistentHitZone persistentHitZone();
			persistentHitZone.m_sName = hitZone.GetName();
			persistentHitZone.m_fHealth = hitZone.GetHealthScaled();

			if (settings.m_bTrimDefaults && float.AlmostEqual(persistentHitZone.m_fHealth, 1.0)) continue;
			if (!settings.m_aHitzoneFilter.IsEmpty() && !settings.m_aHitzoneFilter.Contains(persistentHitZone.m_sName)) continue;

			m_aHitzones.Insert(persistentHitZone);
		}

		if (m_aHitzones.IsEmpty()) return EL_EReadResult.DEFAULT;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		EL_EApplyResult result = EL_EApplyResult.OK;

		array<HitZone> outHitZones();
		HitZoneContainerComponent hitZoneContainer = HitZoneContainerComponent.Cast(worldEntityComponent);
		hitZoneContainer.GetAllHitZones(outHitZones);

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

		return result;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_HitZoneContainerComponentSaveData otherData = EL_HitZoneContainerComponentSaveData.Cast(other);

		if (m_aHitzones.Count() != otherData.m_aHitzones.Count())
			return false;

		foreach (int idx, EL_PersistentHitZone hitZone : m_aHitzones)
		{
			// Try same index first as they are likely to be the correct ones.
			if (hitZone.Equals(otherData.m_aHitzones.Get(idx)))
				continue;

			bool found;
			foreach (int compareIdx, EL_PersistentHitZone otherhitZone : otherData.m_aHitzones)
			{
				if (compareIdx == idx)
					continue; // Already tried in idx direct compare

				if (hitZone.Equals(otherhitZone))
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

class EL_PersistentHitZone
{
	string m_sName;
	float m_fHealth;

	//------------------------------------------------------------------------------------------------
	bool Equals(notnull EL_PersistentHitZone other)
	{
		return m_sName == other.m_sName && float.AlmostEqual(m_fHealth, other.m_fHealth);
	}
};
