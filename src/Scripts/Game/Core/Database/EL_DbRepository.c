class EL_DbRepositoryBase
{
	protected ref EL_DbContext m_DbContext;
	
	void Configure(EL_DbContext dbContext)
	{
		if(m_DbContext) return; //Only allow configure once
		m_DbContext = dbContext;
	}
	
	EL_DbContext GetDbContext()
	{
		return m_DbContext;
	}
	
	// Creation only through EL_DbRepositoryFactory::GetRepository
	protected void EL_DbRepositoryBase();
}

class EL_DbRepository<Class TEntityType> : EL_DbRepositoryBase
{
	typename GetEntityType()
	{
		return TEntityType;
	}

	// ---------------------- Sync API (to use with thread) ----------------------
	
	EL_EDbOperationStatusCode AddOrUpdate(notnull TEntityType entity)
	{
		return m_DbContext.AddOrUpdate(entity);
	}
	
	EL_EDbOperationStatusCode Remove(string entityId)
	{
		return m_DbContext.Remove(TEntityType, entityId);
	}
	
	EL_EDbOperationStatusCode Remove(notnull TEntityType entity)
	{
		return m_DbContext.Remove(TEntityType, entity.GetId());
	}
	
	EL_DbFindResult<TEntityType> Find(string entityId)
	{
		EL_DbFindResult<TEntityType> findResult = FindFirst(EL_DbFind.Id().Equals(entityId));
		
		if(!findResult.Success()) return findResult;
		
		if(!findResult.GetEntity())
		{
			return new EL_DbFindResult<TEntityType>(EL_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND); 
		}
		
		return findResult;
	}
	
	EL_DbFindResult<TEntityType> FindFirst(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null)
	{
		EL_DbFindResults<EL_DbEntity> findResults = m_DbContext.FindAll(TEntityType, condition, orderBy, 1);
		
		if(!findResults.Success())
		{
			return new EL_DbFindResult<TEntityType>(findResults.GetStatusCode());
		}
		
		TEntityType entity = null;
		
		if(findResults.GetEntities().Count() > 0)
		{
			entity = TEntityType.Cast(findResults.GetEntities().Get(0));
		}
		
		return new EL_DbFindResult<TEntityType>(findResults.GetStatusCode(), entity);
	}
	
	EL_DbFindResults<TEntityType> FindAll(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1)
	{
		EL_DbFindResults<EL_DbEntity> findResults = m_DbContext.FindAll(TEntityType, condition, orderBy, limit, offset);

		return new EL_DbFindResults<TEntityType>(findResults.GetStatusCode(), EL_RefArrayCaster<EL_DbEntity, TEntityType>.Convert(findResults.GetEntities()));
	}
	
	// -------------------------------- ASYNC API --------------------------------
	
	void AddOrUpdateAsync(notnull TEntityType entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.AddOrUpdateAsync(entity, callback);
	}
	
	void RemoveAsync(string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.RemoveAsync(TEntityType, entityId, callback);
	}
	
	void RemoveAsync(TEntityType entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.RemoveAsync(TEntityType, entity.GetId(), callback);
	}
	
	void FindAsync(string entityId, EL_DbFindCallbackSingle<TEntityType> callback)
	{
		FindFirstAsync(EL_DbFind.Id().Equals(entityId), null, callback);
	}
	
	void FindFirstAsync(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, EL_DbFindCallbackSingle<TEntityType> callback = null)
	{
		m_DbContext.FindAllAsync(TEntityType, condition, orderBy, 1, -1, callback);
	}
	
	void FindAllAsync(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallback<TEntityType> callback = null)
	{
		m_DbContext.FindAllAsync(TEntityType, condition, orderBy, limit, offset, callback);
	}
	
	protected void EL_DbRepository()
	{
		if(!GetEntityType().IsInherited(EL_DbEntity))
		{
			Debug.Error(string.Format("Db repository '%1' created with entity type '%2' that does not inherit from '%3'. Results will be invalid!", this, TEntityType, EL_DbEntity));
		}
	}
}
