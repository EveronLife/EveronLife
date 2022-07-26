class EL_DbEntityHelper<Class TEntityType>
{
	static EL_DbEntityRepository<TEntityType> GetRepository(string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useDbContextCache = true)
	{
		return EL_DbEntityRepository<TEntityType>.Cast(EL_DbEntityRepositoryFactory.GetRepository(EL_DbEntityRepositoryType.Get(TEntityType), dataSource, useDbContextCache));
	}
}

class EL_DbEntityRepositoryHelper<Class TRepositoryType>
{
	static TRepositoryType Get(string dataSource = EL_DbContextFactory.DEFAULT_SOURCE, bool useDbContextCache = true)
	{
		return TRepositoryType.Cast(EL_DbEntityRepositoryFactory.GetRepository(repositoryType, dataSource, useDbContextCache));
	}
}
