[EL_DbName(EL_PersistentRootEntityCollection, "RootEntityCollection")]
class EL_PersistentRootEntityCollection : EL_DbEntity
{
	ref set<string> m_aRemovedBackedEntities = new set<string>();
	ref map<typename, ref set<string>> m_mAddtionalDynamicEntities = new map<typename, ref set<string>>();
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("dataLayoutVersion", 1);
		saveContext.WriteValue("id", GetId());
		saveContext.WriteValue("removeBakedIds", m_aRemovedBackedEntities);
		
		array<ref EL_SelfSpawnDynamicEntity> addtionalDynamicEntities();
		addtionalDynamicEntities.Resize(m_mAddtionalDynamicEntities.Count());
		
		int idx;
		foreach(typename saveDataType, set<string> ids : m_mAddtionalDynamicEntities)
		{
			EL_SelfSpawnDynamicEntity entry();
			entry.m_sSaveDataType = EL_DbName.Get(saveDataType);
			entry.m_aIds = ids;
			addtionalDynamicEntities.Set(idx++, entry);
		}
		
		saveContext.WriteValue("addtionalDynamicEntities", addtionalDynamicEntities);
		
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
		
		array<ref EL_SelfSpawnDynamicEntity> addtionalDynamicEntities();
		loadContext.ReadValue("addtionalDynamicEntities", addtionalDynamicEntities);
		
		foreach(EL_SelfSpawnDynamicEntity entry : addtionalDynamicEntities)
		{
			m_mAddtionalDynamicEntities.Set(EL_DbName.GetTypeByName(entry.m_sSaveDataType), entry.m_aIds);
		}
		
		return true;
	}	
}

class EL_SelfSpawnDynamicEntity
{
	string m_sSaveDataType;
	ref set<string> m_aIds;
}
