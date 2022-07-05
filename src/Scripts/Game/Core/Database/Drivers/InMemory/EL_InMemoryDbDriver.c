typedef map<string, ref EL_DbEntity> EL_InMemoryDatabaseTable

class EL_InMemoryDatabase
{
	string m_DbName;
	
	ref map<string, ref EL_InMemoryDatabaseTable> m_EntityTables;
	
	void AddOrUpdate(notnull EL_DbEntity entity)
	{
		EL_InMemoryDatabaseTable table = GetTable(entity.Type());
		
		if(table)
		{
			table.Set(entity.m_Id, entity);
		}
	}
	
	void Remove(notnull EL_DbEntity entity)
	{
		EL_InMemoryDatabaseTable table = GetTable(entity.Type());
		
		if(table)
		{
			table.Remove(entity.m_Id);
		}
	}
	
	EL_DbEntity Get(typename entityType, EL_DbEntityId entityId)
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
			m_EntityTables.Set(typeKey, new EL_InMemoryDatabaseTable());
			table = m_EntityTables.Get(typeKey);
		}
		
		return table;
	}
	
	void EL_InMemoryDatabase(string dbName)
	{
		m_DbName = dbName;
	}
}

[EL_DbDriverName(EL_InMemoryDbDriver, {"InMemory"})]
class EL_InMemoryDbDriver : EL_DbDriver
{
	protected static ref map<string, ref EL_InMemoryDatabase> m_Databases;
	
	protected EL_InMemoryDatabase m_Db;
	
	override bool Initalize(string connectionString = string.Empty)
	{
		//Only create the db holder if at least one driver is initalized. (Avoids allocation on clients)
		if(!m_Databases)
		{
			m_Databases = new map<string, ref EL_InMemoryDatabase>();
		}
		
		if(!m_Databases) return false;
		
		//No params yet to the connection data is just the db name
		string dbName = connectionString;
		
		m_Db = m_Databases.Get(dbName);
		
		//Init db if driver was the first one to trying to access it
		if(!m_Db)
		{
			m_Databases.Set(dbName, new EL_InMemoryDatabase(dbName));
			
			//Strong ref is held by map, so we need to get it from there
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
		m_Db.AddOrUpdate(entity);
		
		return EL_DbOperationStatusCode.SUCCESS;
	}
	
	override EL_DbOperationStatusCode RemoveById(typename entityType, EL_DbEntityId entityId)
	{
		EL_DbEntity entity = m_Db.Get(entityType, entityId);
		
		if(!entity) return EL_DbOperationStatusCode.FAILURE_ID_NOT_FOUND;
		
		m_Db.Remove(entity);
		
		return EL_DbOperationStatusCode.SUCCESS;
	}
	
	override array<ref EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, EL_TStringArrayArray orderBy = null, int limit = -1, int offset = -1)
	{
		array<ref EL_DbEntity> dbEntites = m_Db.GetAll(entityType);
		
		array<ref EL_DbEntity> resultEntites();
		
		// TODO apply critera, order, limit, offsets
		
		return resultEntites;
	}
	
	override void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		//In memory is blocking, re-use sync api
		EL_DbOperationStatusCode resultCode = AddOrUpdate(entity);
		
		if(callback)
		{
			callback._SetCompleted(resultCode);
		}
	}

	override void RemoveByIdAsync(typename entityType, EL_DbEntityId entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		//In memory is blocking, re-use sync api
		EL_DbOperationStatusCode resultCode = RemoveById(entityType, entityId);
		
		if(callback)
		{
			callback._SetCompleted(resultCode);
		}
	}

	override void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, EL_TStringArrayArray orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		//In memory is blocking, re-use sync api
		array<ref EL_DbEntity> dbEntites = FindAll(entityType, condition, orderBy, limit, offset);
		
		if(callback)
		{
			callback._SetCompleted(EL_DbOperationStatusCode.SUCCESS, dbEntites);
		}
	}
}
