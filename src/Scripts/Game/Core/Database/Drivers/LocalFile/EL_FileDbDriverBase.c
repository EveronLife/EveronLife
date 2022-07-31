class EL_FileDbDriverBase : EL_DbDriver
{
	static const string DB_BASE_DIR = "$profile:/.db";
	
	// Static props that can be shared across all driver instances
	protected ref EL_DbEntityCache m_EntityCache;
	protected ref map<typename, ref set<string>> m_EntityIds;
	
	protected string m_sDbDir;
	protected bool m_UseCache;
	
	override bool Initalize(string connectionString = string.Empty)
	{
		if(!m_EntityCache) m_EntityCache = new EL_DbEntityCache();
		if(!m_EntityIds) m_EntityIds = new map<typename, ref set<string>>();
		
		FileIO.MakeDirectory(DB_BASE_DIR);
		
		// Placeholder until we either have proper query string parsing or connection settings object
		connectionString.Replace("?cache=false", "");	
		if(connectionString.Replace("?cache=true", "") > 0)
		{
			m_UseCache = true;
		}
		
		string dbName = connectionString;
		
		m_sDbDir = string.Format("%1/%2", DB_BASE_DIR, dbName);
		
		FileIO.MakeDirectory(m_sDbDir);
		
		return FileIO.FileExist(m_sDbDir);
	}
	
	override EL_EDbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		if(!entity.HasId()) return EL_EDbOperationStatusCode.FAILURE_ID_MISSING;
		
		FileIO.MakeDirectory(string.Format("%1/%2", m_sDbDir, EL_DbEntityName.Get(entity.Type())));
		
		EL_EDbOperationStatusCode statusCode = WriteToDisk(entity);
		if(statusCode != EL_EDbOperationStatusCode.SUCCESS) return statusCode;
		
		// TODO: Buffer FileIO and flush later over multiple frames to smooth out db operation spikes
		if(m_UseCache) m_EntityCache.Add(entity);
		
		// Add id to pool of all known ids
		GetIdsByType(entity.Type()).Insert(entity.GetId());
		
		return EL_EDbOperationStatusCode.SUCCESS;
	}
	
	override EL_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		if(!entityId) return EL_EDbOperationStatusCode.FAILURE_ID_MISSING;
		
		EL_EDbOperationStatusCode statusCode = DeleteFromDisk(entityType, entityId);
		if(statusCode != EL_EDbOperationStatusCode.SUCCESS) return statusCode;
		
		if(m_UseCache) m_EntityCache.Remove(entityId);
		
		// Remove id from pool of all known ids
		set<string> ids = GetIdsByType(entityType);
		ids.Remove(ids.Find(entityId));
		
		return EL_EDbOperationStatusCode.SUCCESS;
	}
	
	override EL_DbFindResults<EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		set<string> relevantIds();
		bool needsFilter = false;
		
		// See if we can only load selected few entities by id or we need the entire collection to search through
		if(!condition || !CollectConditionIds(condition, relevantIds))
		{
			// Condition(s) require more information than just ids so all need to be loaded and also filtered by condition
			relevantIds = GetIdsByType(entityType);
			needsFilter = true;
		}
		
		array<ref EL_DbEntity> entities();
		
		foreach(string entityId : relevantIds)
		{
			EL_DbEntity entity;
			
			if(m_UseCache) entity = m_EntityCache.Get(entityId);
			
			if(!entity)
			{
				EL_EDbOperationStatusCode statusCode = ReadFromDisk(entityType, entityId, entity);
				
				// Abort find process if any entity failed
				if(statusCode != EL_EDbOperationStatusCode.SUCCESS || !entity)
				{
					if(statusCode == EL_EDbOperationStatusCode.SUCCESS) statusCode = EL_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;
					return new EL_DbFindResults<EL_DbEntity>(statusCode);
				}
				
				if(m_UseCache) m_EntityCache.Add(entity);
			}
			
			entities.Insert(entity);
		}
		
		if(needsFilter && condition)
		{
			entities = EL_DbFindConditionEvaluator.GetFiltered(entities, condition);
		}
		
		// Order results if wanted
		if(orderBy)
		{
			entities = EL_DbEntitySorter.GetSorted(entities, orderBy);
		}
		
		array<ref EL_DbEntity> resultEntites();
		
		foreach(int idx, EL_DbEntity entity : entities)
		{
			// Respect output limit is specified
			if(limit != -1 && resultEntites.Count() >= limit) break;
			
			// Skip the first n records if offset specified (for paginated loading together with limit)
			if(offset != -1 && idx < offset) continue;
			
			// Return a deep copy so you can not accidentially change the db reference instance in the result handling code
			EL_DbEntity resultDeepCopy = EL_DbEntity.Cast(entityType.Spawn());
			EL_DbEntityUtils.ApplyDbEntityTo(entity, resultDeepCopy);
			
			resultEntites.Insert(resultDeepCopy);
		}
		
		return new EL_DbFindResults<EL_DbEntity>(EL_EDbOperationStatusCode.SUCCESS, resultEntites);
	}
	
	override void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// FileIO is blocking, re-use sync api
		EL_EDbOperationStatusCode resultCode = AddOrUpdate(entity);
		
		if(callback)
		{
			callback._SetCompleted(resultCode);
		}
	}

	override void RemoveAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		// FileIO is blocking, re-use sync api
		EL_EDbOperationStatusCode resultCode = Remove(entityType, entityId);
		
		if(callback)
		{
			callback._SetCompleted(resultCode);
		}
	}

	override void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		// FileIO is blocking, re-use sync api
		EL_DbFindResults<EL_DbEntity> findResults = FindAll(entityType, condition, orderBy, limit, offset);
		
		if(callback)
		{
			callback._SetCompleted(findResults.GetStatusCode(), findResults.GetEntities());
		}
	}
	
	protected set<string> GetIdsByType(typename entityType)
	{
		set<string> ids = m_EntityIds.Get(entityType);
		
		if (!ids)
		{
			ids = GetIdsOnDisk(entityType);
			m_EntityIds.Set(entityType, ids);
		}
		
		return ids;
	}
	
	protected bool CollectConditionIds(EL_DbFindCondition condition, out set<string> ids)
	{
		EL_DbFindFieldStringMultiple stringMultipleCondition = EL_DbFindFieldStringMultiple.Cast(condition);
		if (stringMultipleCondition)
		{
			if (stringMultipleCondition.m_FieldPath != EL_DbEntity.FIELD_ID ||
				stringMultipleCondition.m_ComparisonOperator != EL_EDbFindOperator.EQUAL) return false;
			
			foreach(string id : stringMultipleCondition.m_ComparisonValues)
			{
				ids.Insert(id);
			}
			
			return true;
		}
		
		EL_DbFindConditionWithChildren conditionWithChildren = EL_DbFindConditionWithChildren.Cast(condition);
		if(conditionWithChildren)
		{
			foreach(EL_DbFindCondition childCondition : conditionWithChildren.m_Conditions)
			{
				if(!CollectConditionIds(childCondition, ids)) return false;
			}
			
			return true;
		}
		
		return false;
	}
	
	protected set<string> GetIdsOnDisk(typename entityType)
	{
		EL_FileDbDriverFindIdsCallback callback();
		System.FindFiles(callback.AddFile, m_sDbDir, GetFileExtension());
		
		set<string> ids();
		
		foreach(string id : callback.m_Ids)
		{
			ids.Insert(id);
		}
		
		return ids;
	}
	
	protected string GetFileExtension();
	
	protected EL_EDbOperationStatusCode WriteToDisk(EL_DbEntity entity);

	protected EL_EDbOperationStatusCode ReadFromDisk(typename entityType, string entityId, out EL_DbEntity entity);
	
	protected EL_EDbOperationStatusCode DeleteFromDisk(typename entityType, string entityId)
	{
		string file = string.Format("%1/%2/%3%4", m_sDbDir, EL_DbEntityName.Get(entityType), entityId, GetFileExtension());
		
		if(!FileIO.FileExist(file)) return EL_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND;
		
		if(!FileIO.DeleteFile(file)) return EL_EDbOperationStatusCode.FAILURE_STORAGE_UNAVAILABLE;
		
		return EL_EDbOperationStatusCode.SUCCESS;
	}
	
	EL_DbEntityCache _GetEntityCache()
	{
		return m_EntityCache;
	}
	
	map<typename, ref set<string>> _GetEntityIds()
	{
		return m_EntityIds;
	}
	
	string _GetDbDir()
	{
		return m_sDbDir;
	}
}

class EL_FileDbDriverFindIdsCallback
{
	ref array<string> m_Ids = {};
	
	void AddFile(string fileName, FileAttribute attributes = 0, string filesystem = string.Empty)
	{
		fileName = FilePath.StripPath(fileName);
		fileName = FilePath.StripExtension(fileName);
		
		// Not a UUID of 36 chars length
		if(fileName.IsEmpty() || fileName.Length() != 36) return;
		
		m_Ids.Insert(fileName);
	}
}
