typedef map<string, ref EL_DbEntity> EL_InMemoryDatabaseTable;

class EL_InMemoryDatabase
{
	string m_DbName;
	ref map<string, ref EL_InMemoryDatabaseTable> m_EntityTables;

	//------------------------------------------------------------------------------------------------
	void AddOrUpdate(notnull EL_DbEntity entity)
	{
		EL_InMemoryDatabaseTable table = GetTable(entity.Type());

		if (table)
		{
			table.Set(entity.GetId(), entity);
		}
	}

	//------------------------------------------------------------------------------------------------
	void Remove(notnull EL_DbEntity entity)
	{
		EL_InMemoryDatabaseTable table = GetTable(entity.Type());

		if (table)
		{
			table.Remove(entity.GetId());
		}
	}

	//------------------------------------------------------------------------------------------------
	EL_DbEntity Get(typename entityType, string entityId)
	{
		EL_InMemoryDatabaseTable table = GetTable(entityType);

		if (table)
		{
			return table.Get(entityId);
		}

		return null;
	}

	//------------------------------------------------------------------------------------------------
	array<ref EL_DbEntity> GetAll(typename entityType)
	{
		EL_InMemoryDatabaseTable table = GetTable(entityType);

		array<ref EL_DbEntity> result();

		if (table)
		{
			for (int nElement = 0; nElement < table.Count(); nElement++)
			{
				result.Insert(table.GetElement(nElement));
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected EL_InMemoryDatabaseTable GetTable(typename entityType)
	{
		string typeKey = entityType.ToString();

		EL_InMemoryDatabaseTable table = m_EntityTables.Get(typeKey);

		if (!table)
		{
			table = new EL_InMemoryDatabaseTable();
			m_EntityTables.Set(typeKey, table);
		}

		return table;
	}

	//------------------------------------------------------------------------------------------------
	void EL_InMemoryDatabase(string dbName)
	{
		m_DbName = dbName;
		m_EntityTables = new map<string, ref EL_InMemoryDatabaseTable>();
	}
}
