class EL_DbEntityManager
{
	private EL_DbContext m_DbContext;
	
	void SetDbContext(EL_DbContext dbContext)
	{
		//Only allow it to be set once
		if(m_DbContext) return;
		
		m_DbContext = dbContext;
	}
	
	EL_DbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		return m_DbContext.AddOrUpdate(entity);
	}
	
	EL_DbOperationStatusCode RemoveById(typename entityType, string entityId)
	{
		return m_DbContext.RemoveById(entityType, entityId);
	}
	
	array<ref EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		return m_DbContext.FindAll(entityType, condition, orderBy, limit, offset);
	}
	
	void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.AddOrUpdateAsync(entity, callback);
	}
	
	void RemoveByIdAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.RemoveByIdAsync(entityType, entityId, callback);
	}
	
	void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		m_DbContext.FindAllAsync(entityType, condition, orderBy, limit, offset, callback);
	}
	
	//Entity manager will be constructed through typename.Spawn() depending on the EL_DbEntityRepositoryBase request
	private void EL_DbContext();
}
