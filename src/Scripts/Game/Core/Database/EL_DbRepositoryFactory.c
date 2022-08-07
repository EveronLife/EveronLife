sealed class EL_DbRepositoryFactory
{
	private static ref map<string, ref EL_DbRepositoryBase> s_RepositoryCache;
	
	static void ResetCache()
	{
		s_RepositoryCache = null;
	}
	
	static EL_DbRepositoryBase GetRepository(typename repositoryType, notnull EL_DbContext dbContext)
	{
		EL_DbRepositoryBase repository = null;
		
		if(!s_RepositoryCache)
		{
			s_RepositoryCache = new map<string, ref EL_DbRepositoryBase>();
		}
		
		string cacheKey = string.Format("%1:%2", repositoryType.ToString(), dbContext);
		
		repository = s_RepositoryCache.Get(cacheKey);
		
		// No valid repository found, create a new one
		if(!repository)
		{
			repository = EL_DbRepositoryBase.Cast(repositoryType.Spawn());
			
			if(repository) repository.Configure(dbContext);
		}
		
		// Cache repository to be re-used, even if null because second time it would still create an invalid one
		s_RepositoryCache.Set(cacheKey, repository);
		
		return repository;
	}
}
