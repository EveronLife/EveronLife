[EL_DbName(EL_PersistentRootEntityCollection, "RootEntityCollection")]
class EL_PersistentRootEntityCollection : EL_DbEntity
{
	ref set<string> m_aRemovedBackedEntities = new set<string>();
	ref map<typename, ref set<string>> m_mSelfSpawnDynamicEntities = new map<typename, ref set<string>>();
	
	void Add(EL_PersistenceComponent persistenceComponent, bool baked)
	{
		int idx = m_aRemovedBackedEntities.Find(persistenceComponent.GetPersistentId());
		if(idx != -1) m_aRemovedBackedEntities.Remove(idx);
		
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(persistenceComponent.GetOwner()));
		if(!baked && settings.m_bSelfSpawn)
		{
			set<string> ids = m_mSelfSpawnDynamicEntities.Get(settings.m_tSaveDataTypename);
			
			if(!ids)
			{
				ids = new set<string>();
				m_mSelfSpawnDynamicEntities.Set(settings.m_tSaveDataTypename, ids);
			}

			ids.Insert(persistenceComponent.GetPersistentId());
		}
	}
	
	void Remove(EL_PersistenceComponent persistenceComponent, bool baked)
	{
		if(baked)
		{
			m_aRemovedBackedEntities.Insert(persistenceComponent.GetPersistentId());
			return;
		}
		
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(persistenceComponent.GetComponentData(persistenceComponent.GetOwner()));
		if(settings.m_bSelfSpawn)
		{
			set<string> ids = m_mSelfSpawnDynamicEntities.Get(settings.m_tSaveDataTypename);
		
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
