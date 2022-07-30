class EL_DbContext
{
	protected ref EL_DbDriver m_Driver;
	
	EL_DbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		if(!entity.HasId()) entity.SetId(EL_EntityIdGenerator.Generate());
		return m_Driver.AddOrUpdate(entity);
	}
	
	EL_DbOperationStatusCode Remove(typename entityType, string entityId)
	{
		return m_Driver.Remove(entityType, entityId);
	}
	
	array<ref EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		return m_Driver.FindAll(entityType, condition, orderBy, limit, offset);
	}

	void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		if(!entity.HasId()) entity.SetId(EL_EntityIdGenerator.Generate());
		m_Driver.AddOrUpdateAsync(entity, callback);
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
		if(!_TryGetConnectionString(dataSource, connectionString))
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
	private static bool _TryGetConnectionString(string dataSource, out string connectionString)
	{
		// Todo remove hardcode
		connectionString = string.Format("inmemory://%1?someoption=true&anotheroption=false", dataSource);
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
