typedef map<string, ref EL_DbEntity> EL_InMemoryDatabaseTable;

class EL_InMemoryDatabase
{
	string m_DbName;
	
	ref map<string, ref EL_InMemoryDatabaseTable> m_EntityTables;
	
	void AddOrUpdate(notnull EL_DbEntity entity)
	{
		EL_InMemoryDatabaseTable table = GetTable(entity.Type());
		
		if(table)
		{
			table.Set(entity.GetId(), entity);
		}
	}
	
	void Remove(notnull EL_DbEntity entity)
	{
		EL_InMemoryDatabaseTable table = GetTable(entity.Type());
		
		if(table)
		{
			table.Remove(entity.GetId());
		}
	}
	
	EL_DbEntity Get(typename entityType, string entityId)
	{
		EL_InMemoryDatabaseTable table = GetTable(entityType);
		
		if(table)
		{
			return table.Get(entityId);
		}
		
		return null;
	}
	
	array<ref EL_DbEntity> GetAll(typename entityType)
	{
		EL_InMemoryDatabaseTable table = GetTable(entityType);
		
		array<ref EL_DbEntity> result();
		
		if(table)
		{
			for (int nElement = 0; nElement < table.Count(); nElement++)
			{
				result.Insert(table.GetElement(nElement));
			}
		}
		
		return result;
	}
	
	protected EL_InMemoryDatabaseTable GetTable(typename entityType)
	{
		string typeKey = entityType.ToString();
		
		EL_InMemoryDatabaseTable table = m_EntityTables.Get(typeKey);
		
		if(!table)
		{
			table = new EL_InMemoryDatabaseTable();
			m_EntityTables.Set(typeKey, table);
		}
		
		return table;
	}
	
	void EL_InMemoryDatabase(string dbName)
	{
		m_DbName = dbName;
		
		m_EntityTables = new map<string, ref EL_InMemoryDatabaseTable>();
	}
}

[EL_DbDriverName(EL_InMemoryDbDriver, {"InMemory"})]
class EL_InMemoryDbDriver : EL_DbDriver
{
	protected static ref map<string, ref EL_InMemoryDatabase> m_Databases;
	
	protected EL_InMemoryDatabase m_Db;
	
	protected void ~EL_InMemoryDbDriver()
	{
		Shutdown();
	}
	
	override bool Initalize(string connectionString = string.Empty)
	{
		// Only create the db holder if at least one driver is initalized. (Avoids allocation on clients)
		if(!m_Databases)
		{
			m_Databases = new map<string, ref EL_InMemoryDatabase>();
		}
		
		if(!m_Databases) return false;
		
		// No params yet to the connection data is just the db name
		string dbName = connectionString;
		
		m_Db = m_Databases.Get(dbName);
		
		// Init db if driver was the first one to trying to access it
		if(!m_Db)
		{
			m_Databases.Set(dbName, new EL_InMemoryDatabase(dbName));
			
			// Strong ref is held by map, so we need to get it from there
			m_Db = m_Databases.Get(dbName);
		}
		
		return true;
	}
	
	override void Shutdown()
	{
		if(!m_Databases) return;
		
		if(!m_Db) return;
		
		m_Databases.Remove(m_Db.m_DbName);
	}
	
	override EL_DbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		// Make a copy so after insert you can not accidently change anything on the instance passed into the driver later.
		EL_DbEntity deepCopy = EL_DbEntity.Cast(entity.Type().Spawn());
		EL_DbEntityUtils.ApplyDbEntityTo(entity, deepCopy);
		
		m_Db.AddOrUpdate(deepCopy);
		
		return EL_DbOperationStatusCode.SUCCESS;
	}
	
	override EL_DbOperationStatusCode Remove(typename entityType, string entityId)
	{
		EL_DbEntity entity = m_Db.Get(entityType, entityId);
		
		if(!entity) return EL_DbOperationStatusCode.FAILURE_ID_NOT_FOUND;
		
		m_Db.Remove(entity);
		
		return EL_DbOperationStatusCode.SUCCESS;
	}
	
	override array<ref EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		array<ref EL_DbEntity> entities = m_Db.GetAll(entityType);
		
		// Only continue with those that match the condition if present
		if(condition)
		{
			entities = EL_DbFindConditionEvaluator.GetFiltered(entities, condition);
		}
		
		// Order results if wanted
		if(orderBy)
		{
			entities = EL_DbEntitySorter.GetSorted(entities, orderBy);
		}
		
		array<ref EL_DbEntity> resultEntites();
		
		foreach(int idx, EL_DbEntity entity : entities)
		{
			// Respect output limit is specified
			if(limit != -1 && resultEntites.Count() >= limit) break;
			
			// Skip the first n records if offset specified (for paginated loading together with limit)
			if(offset != -1 && idx < offset) continue;
			
			// Return a deep copy so you can not accidentially change the db reference instance in the result handling code
			EL_DbEntity resultDeepCopy = EL_DbEntity.Cast(entityType.Spawn());
			EL_DbEntityUtils.ApplyDbEntityTo(entity, resultDeepCopy);
			
			resultEntites.Insert(resultDeepCopy);
		}
		
		return resultEntites;
	}
	
	override void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// In memory is blocking, re-use sync api
		EL_DbOperationStatusCode resultCode = AddOrUpdate(entity);
		
		if(callback)
		{
			callback._SetCompleted(resultCode);
		}
	}

	override void RemoveAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// In memory is blocking, re-use sync api
		EL_DbOperationStatusCode resultCode = Remove(entityType, entityId);
		
		if(callback)
		{
			callback._SetCompleted(resultCode);
		}
	}

	override void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		// In memory is blocking, re-use sync api
		array<ref EL_DbEntity> dbEntites = FindAll(entityType, condition, orderBy, limit, offset);
		
		if(callback)
		{
			callback._SetCompleted(EL_DbOperationStatusCode.SUCCESS, dbEntites);
		}
	}
}
