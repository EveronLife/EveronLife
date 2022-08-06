class EL_DbEntityHelper<Class TEntityType>
{
	static EL_DbEntityRepository<TEntityType> GetRepository(notnull EL_DbContext dbContext)
	{
		typename repositoryType = EL_DbEntityRepositoryType.Get(TEntityType); // Can not be inlined or else illegal read happens because of bug in scriptvm.
		return EL_DbEntityRepository<TEntityType>.Cast(EL_DbEntityRepositoryFactory.GetRepository(repositoryType, dbContext));
	}
}

class EL_DbEntityRepositoryHelper<Class TRepositoryType>
{
	static TRepositoryType Get(notnull EL_DbContext dbContext)
	{
		return TRepositoryType.Cast(EL_DbEntityRepositoryFactory.GetRepository(TRepositoryType, dbContext));
	}
}
