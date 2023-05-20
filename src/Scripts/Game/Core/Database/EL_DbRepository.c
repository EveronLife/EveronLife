class EL_DbRepositoryBase
{
	protected ref EL_DbContext m_DbContext;

	//------------------------------------------------------------------------------------------------
	void Configure(EL_DbContext dbContext)
	{
		if (m_DbContext) return; //Only allow configure once
		m_DbContext = dbContext;
	}

	//------------------------------------------------------------------------------------------------
	EL_DbContext GetDbContext()
	{
		return m_DbContext;
	}

	//------------------------------------------------------------------------------------------------
	//! Creation only through EL_DbRepositoryFactory::GetRepository
	protected void EL_DbRepositoryBase();
};

class EL_DbRepository<Class TEntityType> : EL_DbRepositoryBase
{
	//------------------------------------------------------------------------------------------------
	typename GetEntityType()
	{
		return TEntityType;
	}

	// ------------------------------------ Sync API -------------------------------------------------

	//------------------------------------------------------------------------------------------------
	EL_EDbOperationStatusCode AddOrUpdate(notnull TEntityType entity)
	{
		return m_DbContext.AddOrUpdate(entity);
	}

	//------------------------------------------------------------------------------------------------
	EL_EDbOperationStatusCode Remove(string entityId)
	{
		return m_DbContext.Remove(TEntityType, entityId);
	}

	//------------------------------------------------------------------------------------------------
	EL_EDbOperationStatusCode Remove(notnull TEntityType entity)
	{
		return m_DbContext.Remove(TEntityType, entity.GetId());
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindResultSingle<TEntityType> Find(string entityId)
	{
		return FindFirst(EL_DbFind.Id().Equals(entityId));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindResultSingle<TEntityType> FindSingleton()
	{
		EL_DbFindResultSingle<TEntityType> findResult = FindFirst();

		if (!findResult.IsSuccess() || findResult.GetEntity())
			return findResult;

		typename spawnType = TEntityType;
		return new EL_DbFindResultSingle<TEntityType>(EL_EDbOperationStatusCode.SUCCESS, TEntityType.Cast(spawnType.Spawn()));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindResultSingle<TEntityType> FindFirst(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null)
	{
		EL_DbFindResultMultiple<EL_DbEntity> findResults = m_DbContext.FindAll(TEntityType, condition, orderBy, 1);

		if (!findResults.IsSuccess())
			return new EL_DbFindResultSingle<TEntityType>(findResults.GetStatusCode());

		TEntityType entity = null;
		if (findResults.GetEntities().Count() > 0)
			entity = TEntityType.Cast(findResults.GetEntities().Get(0));

		return new EL_DbFindResultSingle<TEntityType>(findResults.GetStatusCode(), entity);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindResultMultiple<TEntityType> FindAll(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1)
	{
		EL_DbFindResultMultiple<EL_DbEntity> findResults = m_DbContext.FindAll(TEntityType, condition, orderBy, limit, offset);
		return new EL_DbFindResultMultiple<TEntityType>(findResults.GetStatusCode(), EL_RefArrayCaster<EL_DbEntity, TEntityType>.Convert(findResults.GetEntities()));
	}

	// ------------------------------------------ ASYNC API ------------------------------------------

	//------------------------------------------------------------------------------------------------
	void AddOrUpdateAsync(notnull TEntityType entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.AddOrUpdateAsync(entity, callback);
	}

	//------------------------------------------------------------------------------------------------
	void RemoveAsync(string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.RemoveAsync(TEntityType, entityId, callback);
	}

	//------------------------------------------------------------------------------------------------
	void RemoveAsync(TEntityType entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.RemoveAsync(TEntityType, entity.GetId(), callback);
	}

	//------------------------------------------------------------------------------------------------
	void FindAsync(string entityId, notnull EL_DbFindCallbackSingle<TEntityType> callback)
	{
		m_DbContext.FindAllAsync(TEntityType, EL_DbFind.Id().Equals(entityId), null, 1, -1, callback);
	}

	//------------------------------------------------------------------------------------------------
	void FindSingletonAsync(notnull EL_DbFindCallbackSingleton<TEntityType> callback)
	{
		m_DbContext.FindAllAsync(TEntityType, null, null, 1, -1, callback);
	}

	//------------------------------------------------------------------------------------------------
	void FindFirstAsync(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, EL_DbFindCallbackSingle<TEntityType> callback = null)
	{
		m_DbContext.FindAllAsync(TEntityType, condition, orderBy, 1, -1, callback);
	}

	//------------------------------------------------------------------------------------------------
	void FindAllAsync(EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, notnull EL_DbFindCallbackMultiple<TEntityType> callback = null)
	{
		m_DbContext.FindAllAsync(TEntityType, condition, orderBy, limit, offset, callback);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_DbRepository()
	{
		if (!GetEntityType().IsInherited(EL_DbEntity))
			Debug.Error(string.Format("Db repository '%1' created with entity type '%2' that does not inherit from '%3'. Results will be invalid!", this, TEntityType, EL_DbEntity));
	}
};
