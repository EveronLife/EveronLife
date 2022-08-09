[EL_DbName(EL_PersistentBakedEntityNameIdMapping, "BakedEntityNameIdMapping")]
class EL_PersistentBakedEntityNameIdMapping : EL_DbEntity
{
	ref map<string, ref Tuple2<string, typename>> m_mNameIdMapping = new map<string, ref Tuple2<string, typename>>();
	protected ref set<string> m_aNamesRequested = new set<string>();
	bool m_bHasChanges;
	
	void Insert(string name, string id, typename saveDataType)
	{
		m_mNameIdMapping.Set(name, new Tuple2<string, typename>(id, saveDataType));
		m_bHasChanges = true;
	}
	
	string GetIdByName(string name)
	{
		m_aNamesRequested.Insert(name);
		Tuple2<string, typename> tuple = m_mNameIdMapping.Get(name);
		if(!tuple) return string.Empty;
		return tuple.param1;
	}
	
	void Save(EL_DbContext dbContext)
	{
		array<string> stale();
			
		foreach(string name, Tuple2<string, typename> tuple : m_mNameIdMapping)
		{
			if(!m_aNamesRequested.Contains(name))
			{
				stale.Insert(name);
				
				// Delete the associated baked entity entry in db for the object that no longer exists
				dbContext.RemoveAsync(tuple.param2, tuple.param1);
			}
		}

		foreach(string removeName : stale)
		{
			m_mNameIdMapping.Remove(removeName);
		}
		
		if(m_mNameIdMapping.Count() == 0)
		{
			// Mapping can be removed when there are no baked map objects with persistence components anymore
			if(GetId()) dbContext.RemoveAsync(this); 
		}
		else if(m_bHasChanges || stale.Count() > 0)
		{
			// If any removals or additions to the mapping happend save for next restart
			dbContext.AddOrUpdateAsync(this);
		}
	}
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("dataLayoutVersion", 1);
		saveContext.WriteValue("id", GetId());
		
		array<ref EL_PersistentBakedEntityNameIdMappingEntry> entries();
		foreach(string name, Tuple2<string, typename> tuple : m_mNameIdMapping)
		{
			EL_PersistentBakedEntityNameIdMappingEntry entry();
			entry.m_sName = name;
			entry.m_sId = tuple.param1;
			entry.m_sSaveDataType = EL_DbName.Get(tuple.param2);
			entries.Insert(entry);
		}
		saveContext.WriteValue("entries", entries);
		
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
		
		array<ref EL_PersistentBakedEntityNameIdMappingEntry> entries();
		loadContext.ReadValue("entries", entries);
		
		foreach(EL_PersistentBakedEntityNameIdMappingEntry entry : entries)
		{
			m_mNameIdMapping.Set(entry.m_sName, new Tuple2<string, typename>(entry.m_sId, EL_DbName.GetTypeByName(entry.m_sSaveDataType)));
		}
		
		return true;
	}
}

class EL_PersistentBakedEntityNameIdMappingEntry
{
	string m_sName;
	string m_sId;
	string m_sSaveDataType;
}
