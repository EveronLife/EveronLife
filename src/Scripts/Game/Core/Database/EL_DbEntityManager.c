class EL_DbEntityManager
{
	private EL_DbContext m_DbContext;
	
	void SetDbContext(EL_DbContext dbContext)
	{
		//Only allow it to be set once
		if(m_DbContext) return;
		
		m_DbContext = dbContext;
	}
	
	void AddOrUpdate(EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.AddOrUpdate(entity, callback);
	}
	
	void RemoveById(EL_DbEntityId entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.RemoveById(entityId, callback);
	}
	
	void FindBy(EL_DbFindCriteria criteria, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		m_DbContext.FindBy(criteria, orderBy, limit, offset, callback);
	}
	
	//Entity manager will be constructed through typename.Spawn() depending on the EL_DbEntityRepositoryBase request
	private void EL_DbContext();
}
