[EL_DbName(EL_PersistentRootEntityCollection, "RootEntityCollection")]
class EL_PersistentRootEntityCollection : EL_DbEntity
{
	ref map<EL_PersistenceComponent, bool> m_mRootPersistenceComponents = new map<EL_PersistenceComponent, bool>();
		
	// Save-data
	ref set<string> m_aRemovedBackedEntities = new set<string>();
	ref map<typename, ref set<string>> m_mSelfSpawnDynamicEntities = new map<typename, ref set<string>>();
	
	void Add(EL_PersistenceComponent persistenceComponent, bool baked, typename selfSpawnType, bool autoSave, EL_EPersistenceManagerState managerState)
	{
		m_mRootPersistenceComponents.Set(persistenceComponent, autoSave);
		
		// Ingore root add during world load as entities that shall be later removed will still try to add themselves
		if(managerState != EL_EPersistenceManagerState.WORLD_INIT)
		{
			int idx = m_aRemovedBackedEntities.Find(persistenceComponent.GetPersistentId());
			if(idx != -1) m_aRemovedBackedEntities.Remove(idx);
		}
		
		if(!baked && selfSpawnType)
		{
			set<string> ids = m_mSelfSpawnDynamicEntities.Get(selfSpawnType);
			
			if(!ids)
			{
				ids = new set<string>();
				m_mSelfSpawnDynamicEntities.Set(selfSpawnType, ids);
			}
			
			ids.Insert(persistenceComponent.GetPersistentId());
		}
	}
	
	void Remove(EL_PersistenceComponent persistenceComponent, bool baked, typename selfSpawnType)
	{
		m_mRootPersistenceComponents.Remove(persistenceComponent);
		
		if(baked)
		{
			m_aRemovedBackedEntities.Insert(persistenceComponent.GetPersistentId());
		}
		else if(selfSpawnType)
		{
			set<string> ids = m_mSelfSpawnDynamicEntities.Get(selfSpawnType);
		
			if(ids)
			{
				int idx = ids.Find(persistenceComponent.GetPersistentId());
				if(idx != -1) ids.Remove(idx);
			}
		}
	}
	
	void Save(EL_DbContext dbContext)
	{
		// Remove collection if it only holds default values
		if (m_aRemovedBackedEntities.Count() == 0 && m_mSelfSpawnDynamicEntities.Count() == 0)
		{
			// Only need to call db if it was previously saved (aka it has an id)
			if(HasId()) dbContext.RemoveAsync(this);
		}
		else
		{
			dbContext.AddOrUpdateAsync(this);
		}
	}
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("dataLayoutVersion", 1);
		saveContext.WriteValue("id", GetId());
		saveContext.WriteValue("removeBakedIds", m_aRemovedBackedEntities);
		
		array<ref EL_SelfSpawnDynamicEntity> selfSpawnDynamicEntities();
		selfSpawnDynamicEntities.Resize(m_mSelfSpawnDynamicEntities.Count());
		
		int idx;
		foreach(typename saveDataType, set<string> ids : m_mSelfSpawnDynamicEntities)
		{
			EL_SelfSpawnDynamicEntity entry();
			entry.m_sSaveDataType = EL_DbName.Get(saveDataType);
			entry.m_aIds = ids;
			selfSpawnDynamicEntities.Set(idx++, entry);
		}
		
		saveContext.WriteValue("selfSpawnDynamicEntities", selfSpawnDynamicEntities);
		
		return true;
	}

	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;
		
		int dataLayoutVersion;
		loadContext.ReadValue("dataLayoutVersion", dataLayoutVersion);
		
		string id;
		loadContext.ReadValue("id", id);
		SetId(id);
		
		loadContext.ReadValue("removeBakedIds", m_aRemovedBackedEntities);
		
		array<ref EL_SelfSpawnDynamicEntity> selfSpawnDynamicEntities();
		loadContext.ReadValue("selfSpawnDynamicEntities", selfSpawnDynamicEntities);
		
		foreach(EL_SelfSpawnDynamicEntity entry : selfSpawnDynamicEntities)
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
