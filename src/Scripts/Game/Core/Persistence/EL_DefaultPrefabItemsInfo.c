class EL_DefaultPrefabItemsInfo
{
	protected static ref map<string, ref EL_DefaultPrefabItemsInfo>> s_mPrefabInfos;

	protected bool m_bReadOnly;
	protected ref map<string, ref array<string>> m_mStorageItems = new map<string, ref array<string>>();

	//------------------------------------------------------------------------------------------------
	static void Add(IEntity prefabChild, InventoryStorageSlot prefabSlot)
	{
		if (!s_mPrefabInfos)
			s_mPrefabInfos = new map<string, ref EL_DefaultPrefabItemsInfo>>();

		string childName = GetPrefabOrBakedName(prefabChild);
		BaseInventoryStorageComponent storage = prefabSlot.GetStorage();
		if (!storage)
			return; // Ignore special cases where sttorage is not known such as magazines and handle them in an inherited implementation

		string prefabParent = GetPrefabOrBakedName(prefabSlot.GetOwner());
		EL_DefaultPrefabItemsInfo info = s_mPrefabInfos.Get(prefabParent);
		if (!info)
		{
			info = new EL_DefaultPrefabItemsInfo();
			s_mPrefabInfos.Set(prefabParent, info);
		}
		else if (info.m_bReadOnly)
		{
			return; //Info for prefab alraedy finalized, a second instance of the prefab needs to be prevented from writing
		}

		string storageKey = string.Format("%1:%2:%3", storage.Type().ToString(), storage.GetPurpose(), storage.GetPriority());
		array<string> prefabChildren = info.m_mStorageItems.Get(storageKey);
		if (!prefabChildren)
		{
			prefabChildren = {};
			info.m_mStorageItems.Set(storageKey, prefabChildren);
		}
		prefabChildren.Insert(childName);
	}

	//------------------------------------------------------------------------------------------------
	static void Finalize(IEntity prefabParent)
	{
		if (!s_mPrefabInfos)
			return;

		EL_DefaultPrefabItemsInfo info = s_mPrefabInfos.Get(GetPrefabOrBakedName(prefabParent));
		if (!info)
			return;

		info.m_bReadOnly = true;
	}

	//------------------------------------------------------------------------------------------------
	static array<string> GetPrefabChildren(BaseInventoryStorageComponent storage)
	{
		if (!s_mPrefabInfos)
			return null;

		string prefabParent = GetPrefabOrBakedName(storage.GetOwner());
		EL_DefaultPrefabItemsInfo info = s_mPrefabInfos.Get(prefabParent);
		if (!info || !info.m_bReadOnly)
			return null;

		string storageKey = string.Format("%1:%2:%3", storage.Type().ToString(), storage.GetPurpose(), storage.GetPriority());
		return info.m_mStorageItems.Get(storageKey);
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_mPrefabInfos = null;
	}

	//------------------------------------------------------------------------------------------------
	protected static string GetPrefabOrBakedName(IEntity entity)
	{
		// Using prefab name will tell us if the entity is fully prefabbed or there are any adjustments
		string name = entity.GetPrefabData().GetPrefabName();
		if (!name)
			name = entity.GetName();

		return name;
	}
};
