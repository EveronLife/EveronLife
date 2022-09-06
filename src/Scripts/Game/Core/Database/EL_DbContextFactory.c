sealed class EL_DbContextFactory
{
	private const string DEFAULT_SOURCE = "db";
	private static ref map<string, ref EL_DbContext> s_mContextCache;

	//------------------------------------------------------------------------------------------------
	//! Gets or creates a databse context instance
	//! \param dataSource Data source name defined in the server configuration
	//! \param useCache If true the context is cached per data source and returned on the next GetContext use
	//! \return context instance or null on missconfiguration of the data source.
	static EL_DbContext GetContext(string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useCache = true)
	{
		EL_DbContext context = null;

		// Use cached instance if allowed
		if (useCache)
		{
			if (!s_mContextCache) s_mContextCache = new map<string, ref EL_DbContext>();

			context = s_mContextCache.Get(dataSource);
		}

		// No valid context found, create a new one
		if (!context)
		{
			context = EL_DbContext._Create(dataSource);
		}

		// Cache context to be re-used if allowed
		if (useCache)
		{
			s_mContextCache.Set(dataSource, context);
		}

		return context;
	}

	//------------------------------------------------------------------------------------------------
	static void ResetCache()
	{
		s_mContextCache = null;
	}
}
