class EL_PersistenceEntityHelper<Class TEntityType>
{
	//------------------------------------------------------------------------------------------------
	//! Get a database repository that is connected to the persistence database source
	//! \return registered or generic respository instance or null on failure
	static EL_DbRepository<TEntityType> GetRepository()
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if (!persistenceManager) return null;

		typename repositoryType = EL_DbRepositoryType.Get(TEntityType); // Can not be inlined or else illegal read happens because of bug in scriptvm.
		if (!repositoryType)
		{
			string repositoryTypeStr = string.Format("EL_DbRepository<%1>", TEntityType);
			Debug.Error(string.Format("Tried to get unknown entity repository type '%1'. Make sure you use it somewhere in your code e.g.: '%1 repository = ...;'", repositoryTypeStr));
		}

		return EL_DbRepository<TEntityType>.Cast(EL_DbRepositoryFactory.GetRepository(repositoryType, persistenceManager.GetDbContext()));
	}
}

class EL_PersistenceRepositoryHelper<Class TRepositoryType>
{
	//------------------------------------------------------------------------------------------------
	//! Get a database repository that is connected to the persistence database source
	//! \return registered or generic respository instance or null on failure
	static TRepositoryType Get()
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if (!persistenceManager) return null;
		
		return TRepositoryType.Cast(EL_DbRepositoryFactory.GetRepository(TRepositoryType, persistenceManager.GetDbContext()));
	}
}
