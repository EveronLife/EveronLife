class EL_DbDriver
{
	static bool s_bForceSyncApi; //Used to forced drivers to rely on sync api during session teardown

	//------------------------------------------------------------------------------------------------
	bool Initalize(string connectionString = string.Empty);

	//------------------------------------------------------------------------------------------------
	void Shutdown();

	//------------------------------------------------------------------------------------------------
	EL_EDbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity);

	//------------------------------------------------------------------------------------------------
	EL_EDbOperationStatusCode Remove(typename entityType, string entityId);

	//------------------------------------------------------------------------------------------------
	EL_DbFindResults<EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1);

	//------------------------------------------------------------------------------------------------
	void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null);

	//------------------------------------------------------------------------------------------------
	void RemoveAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null);

	//------------------------------------------------------------------------------------------------
	void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null);
}
