class EL_DbEntityRepositoryBase
{
	private ref EL_DbContext m_DbContext;
	
	void Configure(EL_DbContext dbContext)
	{
		if(m_DbContext) return; //Only allow configure once
		m_DbContext = dbContext;
	}
	
	EL_DbContext GetDbContext()
	{
		return m_DbContext;
	}

	// Creation only through EL_DbEntityRepositoryFactory::GetRepository
	private void EL_DbEntityRepositoryBase();
}

class EL_DbEntityRepository<Class TEntityType> : EL_DbEntityRepositoryBase
{
	typename GetEntityType()
	{
		return TEntityType;
	}

	// ---------------------- Sync API (to use with thread) ----------------------
	
	EL_EDbOperationStatusCode AddOrUpdate(notnull TEntityType entity)
	{
		return GetDbContext().AddOrUpdate(entity);
	}
	
	EL_EDbOperationStatusCode Remove(string entityId)
	{
		return GetDbContext().Remove(TEntityType, entityId);
	}
	
	EL_EDbOperationStatusCode Remove(notnull TEntityType entity)
	{
		return GetDbContext().Remove(TEntityType, entity.GetId());
	}
	
	EL_DbFindResult<TEntityType> Find(string entityId)
	{
		return FindFirst(EL_DbFind.Id().Equals(entityId));
	}
	
	EL_DbFindResult<TEntityType> FindFirst(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null)
	{
		EL_DbFindResults<EL_DbEntity> findResults = GetDbContext().FindAll(TEntityType, condition, orderBy, 1);
		
		if(findResults.GetEntities().Count() == 1) 
		{
			return new EL_DbFindResult<TEntityType>(findResults.GetStatusCode(), TEntityType.Cast(findResults.GetEntities().Get(0)));
		}
		
		return null;
	}
	
	EL_DbFindResults<TEntityType> FindAll(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1)
	{
		EL_DbFindResults<EL_DbEntity> findResults = GetDbContext().FindAll(TEntityType, condition, orderBy, limit, offset);

		return new EL_DbFindResults<TEntityType>(findResults.GetStatusCode(), EL_RefArrayCaster<EL_DbEntity, TEntityType>.Convert(findResults.GetEntities()));
	}
	
	// -------------------------------- ASYNC API --------------------------------
	
	void AddOrUpdateAsync(notnull TEntityType entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		GetDbContext().AddOrUpdateAsync(entity, callback);
	}
	
	void RemoveAsync(string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		GetDbContext().RemoveAsync(TEntityType, entityId, callback);
	}
	
	void RemoveAsync(TEntityType entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		GetDbContext().RemoveAsync(TEntityType, entity.GetId(), callback);
	}
	
	void FindAsync(string entityId, EL_DbFindCallbackSingle<TEntityType> callback)
	{
		FindFirstAsync(EL_DbFind.Id().Equals(entityId), null, callback);
	}
	
	void FindFirstAsync(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, EL_DbFindCallbackSingle<TEntityType> callback = null)
	{
		GetDbContext().FindAllAsync(TEntityType, condition, orderBy, 1, -1, callback);
	}
	
	void FindAllAsync(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallback<TEntityType> callback = null)
	{
		GetDbContext().FindAllAsync(TEntityType, condition, orderBy, limit, offset, callback);
	}
}
