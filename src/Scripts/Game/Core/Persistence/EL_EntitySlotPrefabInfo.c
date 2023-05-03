class EL_EntitySlotPrefabInfo
{
	protected static ref map<string, ref array<ref Tuple2<string, ResourceName>>> s_mSlotInfoCache;

	//------------------------------------------------------------------------------------------------
	static array<ref Tuple2<string, ResourceName>> GetSlotInfos(notnull IEntity owner, notnull SlotManagerComponent slotManager)
	{
		string key = EL_Utils.GetPrefabOrMapName(owner);
		array<ref Tuple2<string, ResourceName>> infos;
		if (s_mSlotInfoCache)
		{
			infos = s_mSlotInfoCache.Get(key);
		}
		else
		{
			s_mSlotInfoCache = new map<string, ref array<ref Tuple2<string, ResourceName>>>();
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
			bool enabled = false;
			ResourceName prefab = ResourceName.Empty;
			if (slot.Get("Enabled", enabled) && enabled)
				slot.Get("Prefab", prefab);

			infos.Insert(new Tuple2<string, ResourceName>(slot.GetName(), prefab));
		}

		s_mSlotInfoCache.Set(key, infos);

		return infos;
	}
	
	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_mSlotInfoCache = null;
	}
};
