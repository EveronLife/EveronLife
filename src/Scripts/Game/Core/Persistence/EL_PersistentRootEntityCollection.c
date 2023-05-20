[EL_DbName("RootEntityCollection")]
class EL_PersistentRootEntityCollection : EL_MetaDataDbEntity
{
	ref set<string> m_aRemovedBackedRootEntities = new set<string>();
	ref map<typename, ref array<string>> m_mSelfSpawnDynamicEntities = new map<typename, ref array<string>>();

	//------------------------------------------------------------------------------------------------
	void Add(EL_PersistenceComponent persistenceComponent, string persistentId, EL_EPersistenceManagerState state)
	{
		if (EL_BitFlags.CheckFlags(persistenceComponent.GetFlags(), EL_EPersistenceFlags.BAKED))
			return;

		if (state == EL_EPersistenceManagerState.ACTIVE)
		{
			// TODO refactor to use https://feedback.bistudio.com/T172042 when patched
			int idx = m_aRemovedBackedRootEntities.Find(persistentId);
			if (idx != -1)
			{
				m_aRemovedBackedRootEntities.Remove(idx);
				persistenceComponent.FlagAsBaked();
				return;
			}
		}

		EL_PersistenceComponentClass settings = EL_ComponentData<EL_PersistenceComponentClass>.Get(persistenceComponent);
		if (!settings.m_bSelfSpawn)
			return;

		ForceSelfSpawn(persistenceComponent, persistentId, settings);
	}

	//------------------------------------------------------------------------------------------------
	void ForceSelfSpawn(EL_PersistenceComponent persistenceComponent, string persistentId, EL_PersistenceComponentClass settings)
	{
		array<string> ids = m_mSelfSpawnDynamicEntities.Get(settings.m_tSaveDataType);
		if (!ids)
		{
			ids = {};
			m_mSelfSpawnDynamicEntities.Set(settings.m_tSaveDataType, ids);
		}

		ids.Insert(persistentId);
	}

	//------------------------------------------------------------------------------------------------
	void Remove(EL_PersistenceComponent persistenceComponent, string persistentId, EL_EPersistenceManagerState state)
	{
		if (EL_BitFlags.CheckFlags(persistenceComponent.GetFlags(), EL_EPersistenceFlags.BAKED))
		{
			if (state == EL_EPersistenceManagerState.ACTIVE)
				m_aRemovedBackedRootEntities.Insert(persistentId);

			return;
		}

		EL_PersistenceComponentClass settings = EL_ComponentData<EL_PersistenceComponentClass>.Get(persistenceComponent);
		array<string> ids = m_mSelfSpawnDynamicEntities.Get(settings.m_tSaveDataType);
		if (!ids)
			return;

		ids.RemoveItem(persistentId);

		if (ids.IsEmpty())
			m_mSelfSpawnDynamicEntities.Remove(settings.m_tSaveDataType);
	}

	//------------------------------------------------------------------------------------------------
	void Save(EL_DbContext dbContext)
	{
		m_iLastSaved = System.GetUnixTime();

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
		if (!saveContext.IsValid())
			return false;

		SerializeMetaData(saveContext);

		saveContext.WriteValue("m_aRemovedBackedRootEntities", m_aRemovedBackedRootEntities);

		array<ref EL_SelfSpawnDynamicEntity> selfSpawnDynamicEntities();
		selfSpawnDynamicEntities.Reserve(m_mSelfSpawnDynamicEntities.Count());

		foreach (typename saveDataType, array<string> ids : m_mSelfSpawnDynamicEntities)
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
		if (!loadContext.IsValid())
			return false;

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
};

class EL_SelfSpawnDynamicEntity
{
	string m_sSaveDataType;
	ref array<string> m_aIds;
};
