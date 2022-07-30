class EL_DbEntityHelper<Class TEntityType>
{
	static EL_DbEntityRepository<TEntityType> GetRepository(string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useDbContextCache = true)
	{
		typename repositoryType = EL_DbEntityRepositoryType.Get(TEntityType); // Can not be inlined or else illegal read happens because of bug in scriptvm.
		return EL_DbEntityRepository<TEntityType>.Cast(EL_DbEntityRepositoryFactory.GetRepository(repositoryType, dataSource, useDbContextCache));
	}
}

class EL_DbEntityRepositoryHelper<Class TRepositoryType>
{
	static TRepositoryType Get(string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useDbContextCache = true)
	{
		return TRepositoryType.Cast(EL_DbEntityRepositoryFactory.GetRepository(repositoryType, dataSource, useDbContextCache));
	}
}
