class EL_EntitySlotPrefabInfo
{
	protected static ref map<string, ref array<ref EL_EntitySlotPrefabInfo>> s_mSlotInfoCache;

	string m_sName;
	string m_sPivotId;
	vector m_vOffset;
	vector m_vAngles;
	ResourceName m_rPrefab;
	bool m_bEnabled;

	//------------------------------------------------------------------------------------------------
	ResourceName GetEnabledSlotPrefab()
	{
		ResourceName prefab = ResourceName.Empty;
		if (m_bEnabled)
			prefab = m_rPrefab;

		return prefab;
	}

	//------------------------------------------------------------------------------------------------
	static array<ref EL_EntitySlotPrefabInfo> GetSlotInfos(notnull IEntity owner, notnull SlotManagerComponent slotManager)
	{
		string key = EL_Utils.GetPrefabOrMapName(owner);
		array<ref EL_EntitySlotPrefabInfo> infos;
		if (s_mSlotInfoCache)
		{
			infos = s_mSlotInfoCache.Get(key);
		}
		else
		{
			s_mSlotInfoCache = new map<string, ref array<ref EL_EntitySlotPrefabInfo>>();
		}

		if (infos)
			return infos;

		infos = {};
		BaseContainerList slots = slotManager.GetComponentSource(owner).GetObjectArray("Slots");

		int slotsCount = slots.Count();
		infos.Reserve(slotsCount);
		for (int nSlot = 0; nSlot < slotsCount; nSlot++)
		{
			BaseContainer slot = slots.Get(nSlot);

			string pivotId;
			slot.Get("PivotID", pivotId);

			vector offset;
			slot.Get("Offset", offset);

			vector angles;
			slot.Get("Angles", angles);

			ResourceName prefab;
			slot.Get("Prefab", prefab);

			bool enabled;
			slot.Get("Enabled", enabled);

			infos.Insert(new EL_EntitySlotPrefabInfo(slot.GetName(), pivotId, offset, angles, prefab, enabled));
		}

		s_mSlotInfoCache.Set(key, infos);

		return infos;
	}

	//------------------------------------------------------------------------------------------------
	static EL_EntitySlotPrefabInfo GetSlotInfo(notnull IEntity owner, notnull BaseSlotComponent slotComponent)
	{
		return ReadSlot(owner, slotComponent);
	}

	//------------------------------------------------------------------------------------------------
	static EL_EntitySlotPrefabInfo GetSlotInfo(notnull IEntity owner, notnull WeaponSlotComponent slotComponent)
	{
		return ReadSlot(owner, slotComponent);
	}

	//------------------------------------------------------------------------------------------------
	protected static EL_EntitySlotPrefabInfo ReadSlot(IEntity owner, GenericComponent slotComponent)
	{
		BaseContainer prefabInfo = slotComponent.GetComponentSource(owner);

		string pivotId;
		vector offset;
		vector angles;
		ResourceName prefab;
		bool enabled;

		BaseContainer attachSlot = prefabInfo.GetObject("AttachType");
		if (attachSlot)
		{
			attachSlot.Get("PivotID", pivotId);
			attachSlot.Get("Offset", offset);
			attachSlot.Get("Angles", angles);
			attachSlot.Get("Prefab", prefab);
			attachSlot.Get("Enabled", enabled);
		}

		if (!prefab || !enabled)
		{
			if (prefabInfo.Get("EntityPrefab", prefab) && prefab)
				enabled = true;
		}

		return new EL_EntitySlotPrefabInfo(string.Empty, pivotId, offset, angles, prefab, enabled);
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_mSlotInfoCache = null;
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_EntitySlotPrefabInfo(
		string name = string.Empty,
		string pivotId = string.Empty,
		vector offset = vector.Zero,
		vector angles = vector.Zero,
		ResourceName prefab = ResourceName.Empty,
		bool enabled = false)
	{
		m_sName = name;
		m_sPivotId = pivotId;
		m_vOffset = offset;
		m_vAngles = angles;
		m_rPrefab = prefab;
		m_bEnabled = enabled;
	}
};
