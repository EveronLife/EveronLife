class EL_DbEntityHelper<Class TEntityType>
{
	//------------------------------------------------------------------------------------------------
	//! Get the repository responsible to handling the templated entity type
	//! \param dbContext Database context to wrap with the repository
	//! \return database repository instance or null if no type could be found.
	static EL_DbRepository<TEntityType> GetRepository(notnull EL_DbContext dbContext)
	{
		typename repositoryType = EL_DbRepositoryType.Get(TEntityType); // Can not be inlined or else illegal read happens because of bug in scriptvm.
		if (!repositoryType)
		{
			string repositoryTypeStr = string.Format("EL_DbRepository<%1>", TEntityType);
			Debug.Error(string.Format("Tried to get unknown entity repository type '%1'. Make sure you use it somewhere in your code e.g.: '%1 repository = ...;'", repositoryTypeStr));
		}

		return EL_DbRepository<TEntityType>.Cast(EL_DbRepositoryFactory.GetRepository(repositoryType, dbContext));
	}
}

class EL_DbRepositoryHelper<Class TRepositoryType>
{
	//------------------------------------------------------------------------------------------------
	static TRepositoryType Get(notnull EL_DbContext dbContext)
	{
		return TRepositoryType.Cast(EL_DbRepositoryFactory.GetRepository(TRepositoryType, dbContext));
	}
}
