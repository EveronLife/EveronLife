class EL_DbContext
{
	protected ref EL_DbDriver m_Driver;
	
	EL_EDbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		if(!entity.HasId()) entity.SetId(EL_DbEntityIdGenerator.Generate());
		return m_Driver.AddOrUpdate(entity);
	}
	
	EL_EDbOperationStatusCode Remove(notnull EL_DbEntity entity)
	{
		// Save as vars because script vm invalid pointer bug if passed diretly
		typename type = entity.Type();
		string id = entity.GetId();
		return m_Driver.Remove(type, id);
	}
	
	EL_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		return m_Driver.Remove(entityType, entityId);
	}
	
	EL_DbFindResults<EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		return m_Driver.FindAll(entityType, condition, orderBy, limit, offset);
	}

	void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		if(!entity.HasId()) entity.SetId(EL_DbEntityIdGenerator.Generate());

		m_Driver.AddOrUpdateAsync(entity, callback);
	}
	
	void RemoveAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// Save as vars because script vm invalid pointer bug if passed diretly
		typename entityType = entity.Type();
		string entityId = entity.GetId();
		
		m_Driver.RemoveAsync(entityType, entityId, callback);
	}
	
	void RemoveAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_Driver.RemoveAsync(entityType, entityId, callback);
	}
	
	void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		m_Driver.FindAllAsync(entityType, condition, orderBy, limit, offset, callback);
	}
	
	// Internal use only. Use EL_DbContextFactory::GetContext() to get a context instance.
	static EL_DbContext _Create(string dataSource)
	{
		string connectionString;
		if(!TryGetConnectionString(dataSource, connectionString))
		{
			Debug.Error(string.Format("Could not get find connection string for data source '%1'. Please check your server config!", dataSource));
			return null;
		}
		
		int driverEndIdx = connectionString.IndexOf("://");
		if (driverEndIdx == -1)
		{
			Debug.Error(string.Format("Tried to create database context with invalid connection string '%1'.", connectionString));
			return null;
		}
		
		string driverName = connectionString.Substring(0, driverEndIdx);
		string connectionInfo = connectionString.Substring(driverEndIdx + 3, connectionString.Length() - (driverName.Length() + 3));
		
		typename driverType = EL_DbDriverRegistry.Get(driverName);
		if(!driverType.IsInherited(EL_DbDriver))
		{
			Debug.Error(string.Format("Tried to create database context with incompatible driver type '%1'.", driverType));
			return null;
		}
		
		EL_DbDriver driver = EL_DbDriver.Cast(driverType.Spawn());
		if(!driver || !driver.Initalize(connectionInfo))
		{
			Debug.Error(string.Format("Unable to initalize database driver of type '%1'.", driverType));
			return null;
		}
		
		return new EL_DbContext(driver);
	}
	
	// Internal use only
	protected static bool TryGetConnectionString(string dataSource, out string connectionString)
	{
		// Todo remove hardcode
		
		if(dataSource == "testing")
		{
			connectionString = "inmemory://EveronLife";
		}
		else
		{
			connectionString = "jsonfile://EveronLife?cache=true";
		}

		return true;
	}
	
	// Use EL_DbContextFactory::GetContext() to get a context instance.
	protected void EL_DbContext(EL_DbDriver driver)
	{
		m_Driver = driver;
	}
	
	void ~EL_DbContext()
	{
		if(!m_Driver) return;
		m_Driver.Shutdown();
		m_Driver = null;
	}
}
