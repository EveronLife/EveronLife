class EL_DbEntityRepositoryBase
{
	private EL_DbEntityManager m_EntityManager;
	
	protected EL_DbEntityManager GetEntityManager()
	{
		return m_EntityManager;
	}
	
	void Configure(EL_DbContext dbContext)
	{
		// Allow to be called only once
		if(m_EntityManager)
		{
			Debug.Error(string.Format("Tried to configure entity repository '%1' twice. Check stack trace to find and remove miss-use.", ClassName()));
		}
		
		// TODO: Allow each repository to request a different entity manager typename via attribute to selectively enable features such as caching.
		typename managerType = EL_DbEntityManager;
		
		m_EntityManager = EL_DbEntityManager.Cast(managerType.Spawn());
		
		if(m_EntityManager)
		{
			m_EntityManager.SetDbContext(dbContext);
		}
		else
		{
			Debug.Error(string.Format("Tried to create entity repository of type '%1' with invalid entity manager type '%2'.", ClassName(), managerType));
		}
	}
	
	// Creation only through typename.Spawn() and EL_DbEntityRepositoryBase::Setup
	protected void EL_DbEntityRepositoryBase();
}

class EL_DbEntityRepository<Class TEntityType> : EL_DbEntityRepositoryBase
{
	typename GetEntityType()
	{
		return TEntityType;
	}
	
	void AddOrUpdate(TEntityType entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		GetEntityManager().AddOrUpdate(entity, callback);
	}
	
	void RemoveById(EL_DbEntityId entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		GetEntityManager().RemoveById(entityId, callback);
	}

	void Find(EL_DbEntityId entityId, EL_DbFindCallbackSingle<TEntityType> callback)
	{
		// TODO
		//Where(query->field:id->value:id, limit:1).FirstOrDefault();
	}
	
	void FindOneBy(EL_DbFindCriteria criteria, EL_DbFindCallbackSingle<TEntityType> callback)
	{
		// TODO
		//Where(query, limit:1).FirstOrDefault();
	}
	
	void FindBy(EL_DbFindCriteria criteria, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallback<TEntityType> callback = null)
	{
		GetEntityManager().FindBy(criteria, orderBy, limit, offset, callback);
	}
	
	void FindAll(array<ref array<string>> orderBy = null, EL_DbFindCallback<TEntityType> callback = null)
	{
		callback._SetCompleted(EL_DbOperationStatusCode.SUCCESS, new array<ref EL_DbEntity>());
	}
}
