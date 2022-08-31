class EL_FileDbDriverBase : EL_DbDriver
{
	const string DB_BASE_DIR = "$profile:/.db"; //Can be changed through modded class if you want to!

	// Static props that can be shared across all driver instances
	protected ref EL_DbEntityCache m_pEntityCache;
	protected ref map<typename, ref set<string>> m_mEntityIdsyCache;

	protected string m_sDbDir;
	protected bool m_bUseCache;

	//------------------------------------------------------------------------------------------------
	override bool Initalize(string connectionString = string.Empty)
	{
		if (!m_pEntityCache) m_pEntityCache = new EL_DbEntityCache();
		if (!m_mEntityIdsyCache) m_mEntityIdsyCache = new map<typename, ref set<string>>();

		FileIO.MakeDirectory(DB_BASE_DIR);

		// Placeholder until we either have proper query string parsing or connection settings object
		connectionString.Replace("?cache=false", "");
		if (connectionString.Replace("?cache=true", "") > 0)
		{
			m_bUseCache = true;
		}

		string dbName = connectionString;

		m_sDbDir = string.Format("%1/%2", DB_BASE_DIR, dbName);

		FileIO.MakeDirectory(m_sDbDir);

		return FileIO.FileExist(m_sDbDir);
	}

	//------------------------------------------------------------------------------------------------
	override EL_EDbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		if (!entity.HasId()) return EL_EDbOperationStatusCode.FAILURE_ID_NOT_SET;

		FileIO.MakeDirectory(_GetTypeDirectory(entity.Type()));

		EL_EDbOperationStatusCode statusCode = WriteToDisk(entity);
		if (statusCode != EL_EDbOperationStatusCode.SUCCESS) return statusCode;

		if (m_bUseCache) m_pEntityCache.Add(entity);

		// Add id to pool of all known ids
		GetIdsByType(entity.Type()).Insert(entity.GetId());

		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		if (!entityId) return EL_EDbOperationStatusCode.FAILURE_ID_NOT_SET;

		EL_EDbOperationStatusCode statusCode = DeleteFromDisk(entityType, entityId);
		if (statusCode != EL_EDbOperationStatusCode.SUCCESS) return statusCode;

		if (m_bUseCache) m_pEntityCache.Remove(entityId);

		// Remove id from pool of all known ids
		set<string> ids = GetIdsByType(entityType);
		int idx = ids.Find(entityId);
		if (idx >= 0) ids.Remove(idx);

		// If collection of that entity type is empty remove the folder too to keep the file structure clean
		if (ids.IsEmpty())
		{
			FileIO.DeleteFile(_GetTypeDirectory(entityType));
		}

		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EL_DbFindResults<EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		// See if we can only load selected few entities by id or we need the entire collection to search through
		set<string> relevantIds();
		bool needsFilter = false;
		if (!condition || !EL_DbFindCondition.CollectConditionIds(condition, relevantIds))
		{
			// Condition(s) require more information than just ids so all need to be loaded and also filtered by condition
			relevantIds = GetIdsByType(entityType);
			needsFilter = true;
		}

		array<ref EL_DbEntity> entities();

		foreach (string entityId : relevantIds)
		{
			EL_DbEntity entity;

			if (m_bUseCache) entity = m_pEntityCache.Get(entityId);

			if (!entity)
			{
				EL_EDbOperationStatusCode statusCode = ReadFromDisk(entityType, entityId, entity);

				if (statusCode != EL_EDbOperationStatusCode.SUCCESS || !entity) continue;

				if (m_bUseCache) m_pEntityCache.Add(entity);
			}

			entities.Insert(entity);
		}

		if (needsFilter && condition)
		{
			entities = EL_DbFindConditionEvaluator.GetFiltered(entities, condition);
		}

		if (orderBy)
		{
			entities = EL_DbEntitySorter.GetSorted(entities, orderBy);
		}

		array<ref EL_DbEntity> resultEntites();

		foreach (int idx, EL_DbEntity entity : entities)
		{
			// Respect output limit is specified
			if (limit != -1 && resultEntites.Count() >= limit) break;

			// Skip the first n records if offset specified (for paginated loading together with limit)
			if (offset != -1 && idx < offset) continue;

			resultEntites.Insert(entity);
		}

		return new EL_DbFindResults<EL_DbEntity>(EL_EDbOperationStatusCode.SUCCESS, resultEntites);
	}

	//------------------------------------------------------------------------------------------------
	override void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// FileIO is blocking, re-use sync api
		EL_EDbOperationStatusCode resultCode = AddOrUpdate(entity);
		if (callback) callback.Invoke(resultCode);
	}

	//------------------------------------------------------------------------------------------------
	override void RemoveAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// FileIO is blocking, re-use sync api
		EL_EDbOperationStatusCode resultCode = Remove(entityType, entityId);
		if (callback) callback.Invoke(resultCode);
	}

	//------------------------------------------------------------------------------------------------
	override void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		// FileIO is blocking, re-use sync api
		EL_DbFindResults<EL_DbEntity> findResults = FindAll(entityType, condition, orderBy, limit, offset);
		if (callback) callback.Invoke(findResults.GetStatusCode(), findResults.GetEntities());
	}

	//------------------------------------------------------------------------------------------------
	protected set<string> GetIdsByType(typename entityType)
	{
		set<string> ids = m_mEntityIdsyCache.Get(entityType);

		if (!ids)
		{
			ids = GetIdsOnDisk(entityType);
			m_mEntityIdsyCache.Set(entityType, ids);
		}

		return ids;
	}

	//------------------------------------------------------------------------------------------------
	protected set<string> GetIdsOnDisk(typename entityType)
	{
		EL_FileDbDriverFindIdsCallback callback();
		System.FindFiles(callback.AddFile, _GetTypeDirectory(entityType), GetFileExtension());

		set<string> ids();

		foreach (string id : callback.m_Ids)
		{
			ids.Insert(id);
		}

		return ids;
	}

	//------------------------------------------------------------------------------------------------
	string _GetTypeDirectory(typename entityType)
	{
		string entityName = EL_DbName.Get(entityType);

		if (entityName.EndsWith("y"))
		{
			entityName = string.Format("%1ies", entityName.Substring(0, entityName.Length() - 1));
		}
		else
		{
			entityName += "s";
		}

		return string.Format("%1/%2", m_sDbDir, entityName);
	}

	//------------------------------------------------------------------------------------------------
	protected string GetFileExtension();

	//------------------------------------------------------------------------------------------------
	protected EL_EDbOperationStatusCode WriteToDisk(EL_DbEntity entity);

	//------------------------------------------------------------------------------------------------
	protected EL_EDbOperationStatusCode ReadFromDisk(typename entityType, string entityId, out EL_DbEntity entity);

	//------------------------------------------------------------------------------------------------
	protected EL_EDbOperationStatusCode DeleteFromDisk(typename entityType, string entityId)
	{
		string file = string.Format("%1/%2%3", _GetTypeDirectory(entityType), entityId, GetFileExtension());

		if (!FileIO.FileExist(file)) return EL_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND;

		if (!FileIO.DeleteFile(file)) return EL_EDbOperationStatusCode.FAILURE_STORAGE_UNAVAILABLE;

		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	EL_DbEntityCache _GetEntityCache()
	{
		return m_pEntityCache;
	}

	//------------------------------------------------------------------------------------------------
	map<typename, ref set<string>> _GetEntityIds()
	{
		return m_mEntityIdsyCache;
	}
}

class EL_FileDbDriverFindIdsCallback
{
	ref array<string> m_Ids = {};

	//------------------------------------------------------------------------------------------------
	void AddFile(string fileName, FileAttribute attributes = 0, string filesystem = string.Empty)
	{
		fileName = FilePath.StripPath(fileName);
		fileName = FilePath.StripExtension(fileName);

		// Not a UUID of 36 chars length
		if (fileName.IsEmpty() || fileName.Length() != 36) return;

		m_Ids.Insert(fileName);
	}
}
