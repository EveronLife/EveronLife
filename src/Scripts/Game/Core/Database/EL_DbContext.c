class EL_DbContext
{
	protected ref EL_DbDriver m_Driver;
	
	void AddOrUpdate(EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_Driver.AddOrUpdate(entity, callback);
	}
	
	void RemoveById(EL_DbEntityId entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_Driver.RemoveById(entityId, callback);
	}
	
	void FindBy(EL_DbFindCriteria criteria, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		m_Driver.FindBy(criteria, orderBy, limit, offset, callback);
	}
	
	// Internal use only. Use EL_DbContextFactory::GetContext() to get a context instance.
	void EL_DbContext(EL_DbDriver driver)
	{
		m_Driver = driver;
	}
	
	void ~EL_DbContext()
	{
		if(!m_Driver) return;
		m_Driver.Shutdown();
		delete m_Driver;
	}
}
