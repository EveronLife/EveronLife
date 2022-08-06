sealed class EL_DbContextFactory
{
	private const string DEFAULT_SOURCE = "db";
	
	private static ref map<string, ref EL_DbContext> s_ContextCache;

	static void ResetCache()
	{
		s_ContextCache = null;
	}
	
	static EL_DbContext GetContext(string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useCache = true)
	{
		EL_DbContext context = null;
		
		// Use cached instance if allowed
		if(useCache)
		{
			if(!s_ContextCache)
			{
				s_ContextCache = new map<string, ref EL_DbContext>();
				GetGame().m_OnMissionSetInvoker.Insert(ResetCache);
			}
			
			context = s_ContextCache.Get(dataSource);
		}
		
		// No valid context found, create a new one
		if(!context)
		{
			context = EL_DbContext._Create(dataSource);
		}
		
		// Cache context to be re-used if allowed
		if(useCache)
		{
			s_ContextCache.Set(dataSource, context);
		}
		
		return context;
	}
}
