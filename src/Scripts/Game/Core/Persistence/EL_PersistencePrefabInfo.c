class EL_PersistencePrefabInfo
{
	protected static ref map<string, ref EL_PersistencePrefabInfo>> s_mPrefabInfos;

	protected bool m_bReadOnly;
	protected ref map<string, ref array<string>> m_mStorageItems = new map<string, ref array<string>>();

	//------------------------------------------------------------------------------------------------
	static void Add(IEntity prefabChild, InventoryStorageSlot prefabSlot)
	{
		if (!s_mPrefabInfos)
			s_mPrefabInfos = new map<string, ref EL_PersistencePrefabInfo>>();

		string childName = GetPrefabOrBakedName(prefabChild);
		BaseInventoryStorageComponent storage = prefabSlot.GetStorage();
		if (!storage)
			return; // Ignore special cases such as magazines etc and handle them in their concrete storage

		string prefabParent = GetPrefabOrBakedName(prefabSlot.GetOwner());
		EL_PersistencePrefabInfo info = s_mPrefabInfos.Get(prefabParent);
		if (!info)
		{
			info = new EL_PersistencePrefabInfo();
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

		EL_PersistencePrefabInfo info = s_mPrefabInfos.Get(GetPrefabOrBakedName(prefabParent));
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
		EL_PersistencePrefabInfo info = s_mPrefabInfos.Get(prefabParent);
		if (!info || !info.m_bReadOnly)
			return null;

		string storageKey = string.Format("%1:%2:%3", storage.Type().ToString(), storage.GetPurpose(), storage.GetPriority());
		array<string> prefabs = info.m_mStorageItems.Get(storageKey);
		if (!prefabs)
			return null;

		array<string> result();
		result.Copy(prefabs);
		return result;
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_mPrefabInfos = null;
	}

	//------------------------------------------------------------------------------------------------
	protected static string GetPrefabOrBakedName(IEntity entity)
	{
		string name = EL_Utils.GetPrefabName(entity);
		if (!name)
			name = entity.GetName();

		return name;
	}
};
