class EL_DbContextFactory
{
	private const string DEFAULT_SOURCE = "db";
	
	private static ref map<string, ref EL_DbContext> s_ContextCache;
	
	void ResetCache()
	{
		delete s_ContextCache;
	}
	
	static EL_DbContext GetContext(string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useCache = true)
	{
		EL_DbContext context = null;
		
		if(!s_ContextCache)
		{
			s_ContextCache = new map<string, ref EL_DbContext>();
		}
		
		// Use cached instance if allowed
		if(useCache)
		{
			context = s_ContextCache.Get(dataSource);
		}
		
		// No valid context found, create a new one
		if(!context)
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
			if(!driver || driver.Initalize(connectionInfo))
			{
				Debug.Error(string.Format("Unable to initalize database driver of type '%1'.", driverType));
				return null;
			}
			
			context = new EL_DbContext(driver);
		}
		
		// Cache context to be re-used if allowed
		if(useCache)
		{
			s_ContextCache.Set(dataSource, context);
		}
		
		return context;
	}
	
	private static bool TryGetConnectionString(string dataSource, out string connectionString)
	{
		// Todo remove hardcode
		connectionString = string.Format("inmemory://%1?someoption=true&anotheroption=false", dataSource);
		return true;
	}
}
