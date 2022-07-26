sealed class EL_DbEntityRepositoryFactory
{
	private static ref map<typename, ref EL_DbEntityRepositoryBase> s_RepositoryCache;
	
	void ResetCache()
	{
		delete s_RepositoryCache;
	}
	
	static EL_DbEntityRepositoryBase GetRepository(typename repositoryType, string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useDbContextCache = true)
	{
		EL_DbEntityRepositoryBase repository = null;

		// Use cached instance if allowed, depends on if the context can be used from cache, otherwise we need a new instance with a new db context
		if(useDbContextCache)
		{
			if(!s_RepositoryCache)
			{
				s_RepositoryCache = new map<typename, ref EL_DbEntityRepositoryBase>();
			}
			
			repository = s_RepositoryCache.Get(repositoryType);
		}
		
		// No valid repository found, create a new one
		if(!repository)
		{
			EL_DbContext dbContext = EL_DbContextFactory.GetContext(dataSource, useDbContextCache);
			if(!dbContext) return null;
			
			repository = EL_DbEntityRepositoryBase.Cast(repositoryType.Spawn());
			
			if(repository)
			{
				repository.Configure(dbContext);
			}
		}
		
		// Cache repository to be re-used
		if(useDbContextCache)
		{
			s_RepositoryCache.Set(repositoryType, repository);
		}
		
		return repository;
	}
}
