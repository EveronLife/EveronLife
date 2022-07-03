class EL_DbDriver
{
	bool Initalize(string connectionString = string.Empty);
	
	void Shutdown();
	
	void AddOrUpdate(EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null);
	
	void RemoveById(EL_DbEntityId entityId, EL_DbOperationStatusOnlyCallback callback = null);
	
	void FindBy(EL_DbFindCriteria criteria, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null);
}
