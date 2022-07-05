class EL_DbDriver
{
	bool Initalize(string connectionString = string.Empty);
	
	void Shutdown();
	
	EL_DbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity);
	
	EL_DbOperationStatusCode RemoveById(typename entityType, EL_DbEntityId entityId);
	
	array<ref EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, EL_TStringArrayArray orderBy = null, int limit = -1, int offset = -1);
	
	void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null);

	void RemoveByIdAsync(typename entityType, EL_DbEntityId entityId, EL_DbOperationStatusOnlyCallback callback = null);

	void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, EL_TStringArrayArray orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null);
}
