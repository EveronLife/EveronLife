class EL_DbEntityRepositoryBase
{
	private ref EL_DbEntityManager m_EntityManager;
	
	EL_DbEntityManager GetEntityManager()
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

	// ---------------------- Sync API (to use with thread) ----------------------
	
	EL_DbOperationStatusCode AddOrUpdate(notnull TEntityType entity)
	{
		return GetEntityManager().AddOrUpdate(entity);
	}
	
	EL_DbOperationStatusCode Remove(notnull TEntityType entity)
	{
		return GetEntityManager().RemoveById(TEntityType, entity.GetId());
	}
	
	EL_DbOperationStatusCode RemoveById(string entityId)
	{
		return GetEntityManager().RemoveById(TEntityType, entityId);
	}
	
	TEntityType Find(string entityId)
	{
		return FindFirst(EL_DbFind.Field("m_Id").Equals(String(entityId)));
	}
	
	TEntityType FindFirst(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null)
	{
		array<ref EL_DbEntity> findResults = GetEntityManager().FindAll(TEntityType, condition, orderBy, 1);
		
		if(findResults.Count() == 1) return TEntityType.Cast(findResults.Get(0));
		
		return null;
	}
	
	array<ref TEntityType> FindAll(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1)
	{
		array<ref EL_DbEntity> findResults = GetEntityManager().FindAll(TEntityType, condition, orderBy, limit, offset);
		
		return EL_RefArrayCaster<EL_DbEntity, TEntityType>.Convert(findResults);
	}
	
	// -------------------------------- ASYNC API --------------------------------
	
	void AddOrUpdateAsync(notnull TEntityType entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		GetEntityManager().AddOrUpdateAsync(entity, callback);
	}
	
	void RemoveByIdAsync(string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		GetEntityManager().RemoveByIdAsync(TEntityType, entityId, callback);
	}

	void FindAsync(string entityId, EL_DbFindCallbackSingle<TEntityType> callback)
	{
		FindFirstAsync(EL_DbFind.Field("m_Id").Equals(String(entityId)), null, callback);
	}
	
	void FindFirstAsync(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, EL_DbFindCallbackSingle<TEntityType> callback = null)
	{
		GetEntityManager().FindAllAsync(TEntityType, condition, orderBy, 1, -1, callback);
	}
	
	void FindAllAsync(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallback<TEntityType> callback = null)
	{
		GetEntityManager().FindAllAsync(TEntityType, condition, orderBy, limit, offset, callback);
	}
}
