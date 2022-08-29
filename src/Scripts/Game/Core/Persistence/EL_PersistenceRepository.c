class EL_PersistenceRepository<Class TEntityType>
{
	static EL_DbRepository<TEntityType> Get()
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