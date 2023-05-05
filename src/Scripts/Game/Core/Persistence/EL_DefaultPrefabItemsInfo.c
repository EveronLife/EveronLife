class EL_DefaultPrefabItemsInfo
{
	protected static ref map<string, ref EL_DefaultPrefabItemsInfo>> s_mPrefabInfos;

	protected bool m_bReadOnly;
	protected ref map<string, ResourceName> m_mStorageItems = new map<string, ResourceName>();

	//------------------------------------------------------------------------------------------------
	static void Add(IEntity prefabChild, InventoryStorageSlot prefabSlot)
	{
		if (!s_mPrefabInfos)
			s_mPrefabInfos = new map<string, ref EL_DefaultPrefabItemsInfo>>();

		ResourceName childName = EL_Utils.GetPrefabName(prefabChild);
		if (!childName)
			return;

		int slotId = prefabSlot.GetID();
		BaseInventoryStorageComponent storage = prefabSlot.GetStorage();
		if (!storage)
		{
			// TODO: Remove workaround after https://feedback.bistudio.com/T172162 has been adressed
			if (MuzzleComponent.Cast(prefabSlot.GetParentContainer()))
			{
				storage = EL_Component<WeaponAttachmentsStorageComponent>.Find(prefabChild.GetParent());
				slotId = 0;
			}
			else
			{
				return;
			}
		}

		string prefabParent = EL_Utils.GetPrefabOrMapName(prefabSlot.GetOwner());
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

		string storageKey = string.Format("%1:%2:%3:%4", storage.Type().ToString(), storage.GetPurpose(), storage.GetPriority(), slotId);
		info.m_mStorageItems.Set(storageKey, childName);
	}

	//------------------------------------------------------------------------------------------------
	static void Finalize(IEntity prefabParent)
	{
		if (!s_mPrefabInfos)
			return;

		EL_DefaultPrefabItemsInfo info = s_mPrefabInfos.Get(EL_Utils.GetPrefabOrMapName(prefabParent));
		if (!info)
			return;

		info.m_bReadOnly = true;
	}

	//------------------------------------------------------------------------------------------------
	static ResourceName GetDefaultPrefab(BaseInventoryStorageComponent storage, int slotId)
	{
		ResourceName result;

		if (s_mPrefabInfos)
		{
			string prefabParent = EL_Utils.GetPrefabOrMapName(storage.GetOwner());
			EL_DefaultPrefabItemsInfo info = s_mPrefabInfos.Get(prefabParent);
			if (info && info.m_bReadOnly)
			{
				string storageKey = string.Format("%1:%2:%3:%4", storage.Type().ToString(), storage.GetPurpose(), storage.GetPriority(), slotId);
				result = info.m_mStorageItems.Get(storageKey);
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_mPrefabInfos = null;
	}
};
