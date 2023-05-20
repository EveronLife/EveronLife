[EL_DbConnectionInfoDriverType(EL_InMemoryDbDriver), BaseContainerProps()]
class EL_InMemoryDbConnectionInfo : EL_DbConnectionInfoBase
{
};

[EL_DbDriverName({"InMemory"})]
class EL_InMemoryDbDriver : EL_DbDriver
{
	protected static ref map<string, ref EL_InMemoryDatabase> s_mDatabases;
	protected EL_InMemoryDatabase m_pDb;

	//------------------------------------------------------------------------------------------------
	override bool Initalize(notnull EL_DbConnectionInfoBase connectionInfo)
	{
		// Only create the db holder if at least one driver is initalized. (Avoids allocation on clients)
		if (!s_mDatabases)
			s_mDatabases = new map<string, ref EL_InMemoryDatabase>();

		string dbName = connectionInfo.m_sDatabaseName;
		m_pDb = s_mDatabases.Get(dbName);

		// Init db if driver was the first one to trying to access it
		if (!m_pDb)
		{
			s_mDatabases.Set(dbName, new EL_InMemoryDatabase(dbName));
			m_pDb = s_mDatabases.Get(dbName); // Strong ref held by map so get it there
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EDbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		if (!entity.HasId())
			return EL_EDbOperationStatusCode.FAILURE_ID_NOT_SET;

		// Make a copy so after insert you can not accidently change anything on the instance passed into the driver later.
		EL_DbEntity deepCopy = EL_DbEntity.Cast(entity.Type().Spawn());
		EL_DbEntityUtils.StructAutoCopy(entity, deepCopy);

		m_pDb.AddOrUpdate(deepCopy);
		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		if (!entityId)
			return EL_EDbOperationStatusCode.FAILURE_ID_NOT_SET;

		m_pDb.Remove(entityType, entityId);
		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EL_DbFindResultMultiple<EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		array<ref EL_DbEntity> entities;

		// See if we can only load selected few entities by id or we need the entire collection to search through
		set<string> relevantIds();
		bool needsFilter = false;
		if (condition && EL_DbFindCondition.CollectConditionIds(condition, relevantIds))
		{
			entities = {};
			foreach (string relevantId : relevantIds)
			{
				EL_DbEntity entity = m_pDb.Get(entityType, relevantId);
				if (entity)
					entities.Insert(entity);
			}
		}
		else
		{
			entities = m_pDb.GetAll(entityType);
			needsFilter = true;
		}

		if (needsFilter && condition)
			entities = EL_DbFindConditionEvaluator.GetFiltered(entities, condition);

		if (orderBy)
			entities = EL_DbEntitySorter.GetSorted(entities, orderBy);

		array<ref EL_DbEntity> resultEntites();

		foreach (int idx, EL_DbEntity entity : entities)
		{
			// Respect output limit is specified
			if (limit != -1 && resultEntites.Count() >= limit)
				break;

			// Skip the first n records if offset specified (for paginated loading together with limit)
			if (offset != -1 && idx < offset)
				continue;

			// Return a deep copy so you can not accidentially change the db reference instance in the result handling code
			EL_DbEntity deepCopy = EL_DbEntity.Cast(entityType.Spawn());
			EL_DbEntityUtils.StructAutoCopy(entity, deepCopy);

			resultEntites.Insert(deepCopy);
		}

		return new EL_DbFindResultMultiple<EL_DbEntity>(EL_EDbOperationStatusCode.SUCCESS, resultEntites);
	}

	//------------------------------------------------------------------------------------------------
	override void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// In memory is blocking, re-use sync api
		EL_EDbOperationStatusCode resultCode = AddOrUpdate(entity);
		if (callback)
			callback.Invoke(resultCode);
	}

	//------------------------------------------------------------------------------------------------
	override void RemoveAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// In memory is blocking, re-use sync api
		EL_EDbOperationStatusCode resultCode = Remove(entityType, entityId);
		if (callback)
			callback.Invoke(resultCode);
	}

	//------------------------------------------------------------------------------------------------
	override void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		// In memory is blocking, re-use sync api
		EL_DbFindResultMultiple<EL_DbEntity> findResults = FindAll(entityType, condition, orderBy, limit, offset);
		if (callback)
			callback.Invoke(findResults.GetStatusCode(), findResults.GetEntities());
	}

	//------------------------------------------------------------------------------------------------
	void ~EL_InMemoryDbDriver()
	{
		if	(s_mDatabases && m_pDb)
			s_mDatabases.Remove(m_pDb.m_DbName);
	}
};
