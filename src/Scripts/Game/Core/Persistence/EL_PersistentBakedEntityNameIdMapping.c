[EL_DbName(EL_PersistentBakedEntityNameIdMapping, "BakedEntityNameIdMapping")]
class EL_PersistentBakedEntityNameIdMapping : EL_MetaDataDbEntity
{
	ref set<string> m_aBakedNames = new set<string>();

	ref map<string, ref Tuple2<string, typename>> m_mNameIdMapping = new map<string, ref Tuple2<string, typename>>();
	protected ref set<string> m_aNamesRequested = new set<string>();
	protected bool m_bHasChanges;

	//------------------------------------------------------------------------------------------------
	bool Contains(string name)
	{
		return m_mNameIdMapping.Contains(name);
	}

	//------------------------------------------------------------------------------------------------
	void Insert(string name, string id, typename saveDataType)
	{
		m_mNameIdMapping.Set(name, new Tuple2<string, typename>(id, saveDataType));
		m_bHasChanges = true;
	}

	//------------------------------------------------------------------------------------------------
	string GetIdByName(string name)
	{
		m_aNamesRequested.Insert(name);
		Tuple2<string, typename> tuple = m_mNameIdMapping.Get(name);
		if (!tuple) return string.Empty;
		return tuple.param1;
	}

	//------------------------------------------------------------------------------------------------
	void Save(EL_DbContext dbContext)
	{
		m_iLastSaved = EL_DateTimeUtcAsInt.Now();

		array<string> stale();

		foreach (string name, Tuple2<string, typename> tuple : m_mNameIdMapping)
		{
			if (!m_aNamesRequested.Contains(name))
			{
				stale.Insert(name);

				// Delete the associated baked entity entry in db for the object that no longer exists
				dbContext.RemoveAsync(tuple.param2, tuple.param1);
			}
		}

		foreach (string removeName : stale)
		{
			m_mNameIdMapping.Remove(removeName);
		}

		if (m_mNameIdMapping.IsEmpty())
		{
			// Mapping can be removed when there are no baked map objects with persistence components anymore
			if (GetId()) dbContext.RemoveAsync(this);
		}
		else if (m_bHasChanges || stale.Count() > 0)
		{
			// If any removals or additions to the mapping happend save for next restart
			dbContext.AddOrUpdateAsync(this);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		SerializeMetaData(saveContext);

		map<typename, ref EL_BakedEntityNameIdMappingType> typeMap();

		foreach (string name, Tuple2<string, typename> tuple : m_mNameIdMapping)
		{
			EL_BakedEntityNameIdMappingType type = typeMap.Get(tuple.param2);
			if (!type)
			{
				type = new EL_BakedEntityNameIdMappingType();
				type.m_sSaveDataType = EL_DbName.Get(tuple.param2);
				typeMap.Set(tuple.param2, type);
			}

			array<ref EL_BakedEntityNameIdMappingEntry> entries = type.m_aEntries;
			if (!entries)
			{
				entries = {};
				type.m_aEntries = entries;
			}

			EL_BakedEntityNameIdMappingEntry entry();
			entry.m_sName = name;
			entry.m_sId = tuple.param1;
			entries.Insert(entry);
		}

		array<ref EL_BakedEntityNameIdMappingType> types();
		foreach (auto _, EL_BakedEntityNameIdMappingType type : typeMap)
		{
			types.Insert(type);
		}

		saveContext.WriteValue("m_aTypes", types);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		DeserializeMetaData(loadContext);

		array<ref EL_BakedEntityNameIdMappingType> types();
		loadContext.ReadValue("m_aTypes", types);

		foreach (EL_BakedEntityNameIdMappingType type : types)
		{
			foreach (EL_BakedEntityNameIdMappingEntry entry : type.m_aEntries)
			{
				m_mNameIdMapping.Set(entry.m_sName, new Tuple2<string, typename>(entry.m_sId, EL_DbName.GetTypeByName(type.m_sSaveDataType)));
			}
		}

		return true;
	}
};

class EL_BakedEntityNameIdMappingType
{
	string m_sSaveDataType;
	ref array<ref EL_BakedEntityNameIdMappingEntry> m_aEntries;
};

class EL_BakedEntityNameIdMappingEntry
{
	string m_sName;
	string m_sId;
};
