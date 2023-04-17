[EL_DbName(EL_PersistentRootEntityCollection, "RootEntityCollection")]
class EL_PersistentRootEntityCollection : EL_MetaDataDbEntity
{
	ref set<string> m_aBakedRootEntityIds = new set<string>();
	ref set<string> m_aRemovedBackedRootEntities = new set<string>();
	ref map<typename, ref set<string>> m_mSelfSpawnDynamicEntities = new map<typename, ref set<string>>();

	//------------------------------------------------------------------------------------------------
	void Add(EL_PersistenceComponent persistenceComponent, string persistentId, EL_EPersistenceManagerState state)
	{
		if (state < EL_EPersistenceManagerState.SETUP)
		{
			m_aBakedRootEntityIds.Insert(persistentId);
			return;
		}
		else if (m_aBakedRootEntityIds.Contains(persistentId))
		{
			int idx = m_aRemovedBackedRootEntities.Find(persistentId);
			if (idx != -1)
			{
				m_aRemovedBackedRootEntities.Remove(idx);
				//PrintFormat("Re-Added baked %1(%2).", EL_Utils.GetPrefabName(persistenceComponent.GetOwner()), persistentId);
			}
			return;
		}

        EL_PersistenceComponentClass settings = EL_ComponentData<EL_PersistenceComponentClass>.Get(persistenceComponent);
		if (!settings.m_bSelfSpawn) return;

		set<string> ids = m_mSelfSpawnDynamicEntities.Get(settings.m_tSaveDataTypename);
		if (!ids)
		{
			ids = new set<string>();
			m_mSelfSpawnDynamicEntities.Set(settings.m_tSaveDataTypename, ids);
		}
		ids.Insert(persistentId);
		//PrintFormat("Added dynamic spawn %1(%2).", EL_Utils.GetPrefabName(persistenceComponent.GetOwner()), persistentId);
	}

	//------------------------------------------------------------------------------------------------
	void Remove(EL_PersistenceComponent persistenceComponent, string persistentId, EL_EPersistenceManagerState state)
	{
		if (m_aBakedRootEntityIds.Contains(persistentId))
		{
			//PrintFormat("Removed backed %1(%2).", EL_Utils.GetPrefabName(persistenceComponent.GetOwner()), persistentId);
			m_aRemovedBackedRootEntities.Insert(persistentId);
			return; // Skip rest for baked entities
		}

        EL_PersistenceComponentClass settings = EL_ComponentData<EL_PersistenceComponentClass>.Get(persistenceComponent);
		set<string> ids = m_mSelfSpawnDynamicEntities.Get(settings.m_tSaveDataTypename);
		if (!ids) return;

		int idx = ids.Find(persistentId);
		if (idx != -1) ids.Remove(idx);
		if (ids.IsEmpty()) m_mSelfSpawnDynamicEntities.Remove(settings.m_tSaveDataTypename);

		//PrintFormat("Removed dynamic spawn %1(%2).", EL_Utils.GetPrefabName(persistenceComponent.GetOwner()), persistentId);
	}

	//------------------------------------------------------------------------------------------------
	void Save(EL_DbContext dbContext)
	{
		// Remove collection if it only holds default values and it was previously saved (aka it has an id)
		if (m_aRemovedBackedRootEntities.IsEmpty() && m_mSelfSpawnDynamicEntities.IsEmpty())
		{
			if (HasId()) dbContext.RemoveAsync(this); //Only actually remove if id indicates it was saved before.
		}
		else
		{
			dbContext.AddOrUpdateAsync(this);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		SerializeMetaData(saveContext);

		saveContext.WriteValue("m_aRemovedBackedRootEntities", m_aRemovedBackedRootEntities);

		array<ref EL_SelfSpawnDynamicEntity> selfSpawnDynamicEntities();
		selfSpawnDynamicEntities.Reserve(m_mSelfSpawnDynamicEntities.Count());

		foreach (typename saveDataType, set<string> ids : m_mSelfSpawnDynamicEntities)
		{
			EL_SelfSpawnDynamicEntity entry();
			entry.m_sSaveDataType = EL_DbName.Get(saveDataType);
			entry.m_aIds = ids;
			selfSpawnDynamicEntities.Insert(entry);
		}

		saveContext.WriteValue("m_aSelfSpawnDynamicEntities", selfSpawnDynamicEntities);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		DeserializeMetaData(loadContext);

		loadContext.ReadValue("m_aRemovedBackedRootEntities", m_aRemovedBackedRootEntities);

		array<ref EL_SelfSpawnDynamicEntity> selfSpawnDynamicEntities();
		loadContext.ReadValue("m_aSelfSpawnDynamicEntities", selfSpawnDynamicEntities);

		foreach (EL_SelfSpawnDynamicEntity entry : selfSpawnDynamicEntities)
		{
			m_mSelfSpawnDynamicEntities.Set(EL_DbName.GetTypeByName(entry.m_sSaveDataType), entry.m_aIds);
		}

		return true;
	}
}

class EL_SelfSpawnDynamicEntity
{
	string m_sSaveDataType;
	ref set<string> m_aIds;
}
