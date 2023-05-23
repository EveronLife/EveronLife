/*
overall goal:
				- not query records we have the latest info for or that are deleted
				- be able to return results for stuff that was not yet flushed to db

Or(
				Id().EqualsAnyOf({..., xxxxxx-xxxxx-xxxxx, ...}),
				And(
								Field("something").GreaterThan(5),
								Id().Equals("xxxxx-xxxx-xxxxx")
				)
)

---

>>> Id().EqualsAnyOf({..., xxxxxx-xxxxx-xxxxx, known-id-from-cache}),

And(
				Id().EqualsAnyOf({..., xxxxxx-xxxxx-xxxxx, known-id-from-cache}),
				Id().Not().EqualsAnyOf({xxxxxx-xxxxx-xxxxx, known-id-from-cache}),
)

----

not() does not matter, won't affect our cache in any way?

if id equals as part of OR remove the entry. (bonus to reduce or to just the single statement later)
Or(
				And(
								Field("something").GreaterThan(5),
								Id().Equals("xxxx-xxxx-xxxx")
				),
				And(
								Field("something").GreaterThan(7),
								Id().EqualsAnyOf("yyyy-yyyy-yyyy", "zzzz-zzzz-zzzz")
				),
)

if id as part of And
				1. if that id is removed, delete whole And(), can never return anything.
				2. id is something we updated, then apply the condition onto the db entity. if success evalute entity directly or just let query pass?!?
								-> just count it as not id only, and let it run though in memory and then real driver with not(deleted, updated) extra clause.

----

1. remove any id conditions for already removed entries
				-> recurse, all by id, remove ids that are removed.
								-> if empty remove condition

2. check if id only query
				-> if yes, grab queried ids and answer all results we know from memory
								-> if there are any ids we do not know, go to actual driver to retrieve
				-> if by other field do driver query, then also do memory query for that entitytype
								-> add condition that the result is none of the removed or yet pending update ids (this way no outdated info we already know will be flushed away is returned from driver)
								-> replace driver results with any matching ids from memory results
								-> remove any results that are already deleted



ways to still fix this "broken" concept:

- if limit or offset, send original condition and post process order and limit and replace and remove entires
				.... except I still need to add new records ... so how do they know where to go ...

- dumb query everything from memory and then from driver, and then replace and remove results

- Get all from dB, no sort. Add in memory, then sort and apply limits and offset ... Again I need to know total count of collection. Count could change in dB without game knowing ...

- Another possibility is to flush all pending changes before a complex query and then just sent to to dB. Can saving there cause inconsistencies? Maybe not if flush all changes and not just current entity type?

*/

/*
class EL_DbDriverBufferWrapper : EL_DbDriver
{
	protected ref EL_DbDriver m_pDriver;

	protected ref map<string, int> m_mRemoveBufferIndices;
	protected ref map<typename, ref array<string>> m_mRemoveBufferIds;
	protected ref map<typename, ref array<bool>> m_mRemoveBufferAsync;

	protected ref map<string, int> m_mAddOrUpdateBufferIndices;
	protected ref map<typename, ref array<string>> m_mAddOrUpdateBufferIds;
	protected ref map<typename, ref array<bool>> m_mAddOrUpdateBufferAsync;
	protected ref map<typename, ref array<ref EL_DbEntity>> m_mAddOrUpdateBufferEntities;

	protected bool m_bFlushActive;

	//------------------------------------------------------------------------------------------------
	override EL_EDbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		return AddOrUpdate(entity, false);
	}

	//------------------------------------------------------------------------------------------------
	override EL_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		return Remove(entityType, entityId, false);
	}

	//------------------------------------------------------------------------------------------------
	override EL_DbFindResultMultiple<EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		return FindAllFromBuffer(entityType, condition, orderBy, limit, offset, false, null);
	}

	//------------------------------------------------------------------------------------------------
	override void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		AddOrUpdate(entity, true);
		if (callback)
			callback.Invoke(EL_EDbOperationStatusCode.SUCCESS);
	}

	//------------------------------------------------------------------------------------------------
	override void RemoveAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		Remove(entityType, entityId, true);
		if (callback)
			callback.Invoke(EL_EDbOperationStatusCode.SUCCESS);
	}

	//------------------------------------------------------------------------------------------------
	override void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		FindAllFromBuffer(entityType, condition, orderBy, limit, offset, true, callback);
	}

	//------------------------------------------------------------------------------------------------
	void Flush(int maxBatchSize = 50, bool forceBlocking = false)
	{
		if (m_bFlushActive)
			return;

		m_bFlushActive = true;
		if (!s_bForceBlocking && !forceBlocking)
		{
			thread FlushTick(maxBatchSize);
		}
		else
		{
			FlushTick(maxBatchSize, true);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void FlushTick(int maxBatchSize, bool forceBlocking = false)
	{
		int operation;

		foreach (typename entityType, array<string> ids : m_mRemoveBufferIds)
		{
			array<bool> asyncArray = m_mRemoveBufferAsync.Get(entityType);

			foreach (int idx, string id : ids)
			{
				if (!id)
					continue;

				if (asyncArray.Get(idx))
				{
					m_pDriver.RemoveAsync(entityType, id);
				}
				else
				{
					m_pDriver.Remove(entityType, id);
				}

				if ((++operation % maxBatchSize == 0) && !forceBlocking)
					Sleep(100);
			}
		}

		m_mRemoveBufferIds.Clear();
		m_mRemoveBufferAsync.Clear();
		m_mRemoveBufferIndices.Clear();

		foreach (typename entityType, array<ref EL_DbEntity> entities : m_mAddOrUpdateBufferEntities)
		{
			array<bool> asyncArray = m_mAddOrUpdateBufferAsync.Get(entityType);

			foreach (int idx, EL_DbEntity entity : entities)
			{
				if (!entity)
					continue;

				if (asyncArray.Get(idx))
				{
					m_pDriver.AddOrUpdateAsync(entity);
				}
				else
				{
					m_pDriver.AddOrUpdate(entity);
				}

				if ((++operation % maxBatchSize == 0) && !forceBlocking)
					Sleep(100);
			}
		}

		m_mAddOrUpdateBufferIds.Clear();
		m_mAddOrUpdateBufferAsync.Clear();
		m_mAddOrUpdateBufferIndices.Clear();
		m_mAddOrUpdateBufferEntities.Clear();

		m_bFlushActive = false;
	}

	//------------------------------------------------------------------------------------------------
	protected EL_EDbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity, bool async)
	{
		string id = entity.GetId();
		typename entityType = entity.Type();

		int removeIdx;
		if (m_mRemoveBufferIndices.Find(id, removeIdx))
		{
			m_mRemoveBufferIds.Get(entityType).Set(removeIdx, string.Empty);
			m_mRemoveBufferIndices.Remove(id);
		}

		int addIdx;
		if (m_mAddOrUpdateBufferIndices.Find(id, addIdx))
		{
			// Replace existing entry
			m_mAddOrUpdateBufferEntities.Get(entityType).Set(addIdx, entity);
			m_mAddOrUpdateBufferAsync.Get(entityType).Set(addIdx, async);
		}
		else
		{
			array<ref EL_DbEntity> entities = m_mAddOrUpdateBufferEntities.Get(entityType);
			if (!entities)
			{
				entities = {};
				m_mAddOrUpdateBufferEntities.Set(entityType, entities);
			}
			addIdx = entities.Insert(entity);

			array<bool> asyncBuffer = m_mAddOrUpdateBufferAsync.Get(entityType);
			if (!asyncBuffer)
			{
				asyncBuffer = {};
				m_mAddOrUpdateBufferAsync.Set(entityType, asyncBuffer);
			}
			if (addIdx >= asyncBuffer.Count())
				asyncBuffer.Resize(addIdx + 1);

			asyncBuffer.Set(addIdx, async);

			array<string> ids = m_mAddOrUpdateBufferIds.Get(entityType);
			if (!ids)
			{
				ids = {};
				m_mAddOrUpdateBufferIds.Set(entityType, ids);
			}
			if (addIdx >= ids.Count())
				ids.Resize(addIdx + 1);

			ids.Set(addIdx, id);

			m_mAddOrUpdateBufferIndices.Set(id, addIdx);
		}

		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	protected EL_EDbOperationStatusCode Remove(typename entityType, string entityId, bool async)
	{
		int addIdx;
		if (m_mAddOrUpdateBufferIndices.Find(entityId, addIdx))
		{
			m_mAddOrUpdateBufferEntities.Get(entityType).Set(addIdx, null);
			m_mAddOrUpdateBufferIndices.Remove(entityId);
		}

		int removeIdx;
		if (!m_mRemoveBufferIndices.Find(entityId, removeIdx))
		{
			array<string> removals = m_mRemoveBufferIds.Get(entityType);
			if (!removals)
			{
				removals = {};
				m_mRemoveBufferIds.Set(entityType, removals);
			}
			removeIdx = removals.Insert(entityId);

			array<bool> asyncBuffer = m_mRemoveBufferAsync.Get(entityType);
			if (!asyncBuffer)
			{
				asyncBuffer = {};
				m_mRemoveBufferAsync.Set(entityType, asyncBuffer);
			}
			if (removeIdx >= asyncBuffer.Count())
				asyncBuffer.Resize(removeIdx + 1);

			asyncBuffer.Set(removeIdx, async);

			m_mRemoveBufferIndices.Set(entityId, removeIdx);
		}

		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	protected EL_DbFindResultMultiple<EL_DbEntity> FindAllFromBuffer(
		typename entityType,
		EL_DbFindCondition condition,
		array<ref TStringArray> orderBy,
		int limit,
		int offset,
		bool async,
		EL_DbFindCallbackBase callback)
	{
		array<string> removedIds = m_mRemoveBufferIds.Get(entityType);
		array<ref EL_DbEntity> entities = m_mAddOrUpdateBufferEntities.Get(entityType);

		array<ref EL_DbEntity> bufferedResults();
		EL_DbFindCondition processedCondition = condition;

		array<ref TStringArray> orderByDriver = orderBy;

		int driverLimit = limit;
		int driverOffset = offset;
		int compenstatedOffset = offset;

		if (removedIds || entities)
		{
			array<string> queriedIds();
			bool boolComplexCondition;
			processedCondition = ProcessCondition(entityType, condition, false, queriedIds, boolComplexCondition);

			if (!boolComplexCondition && condition)
			{
				// go to cache and get ids
				array<string> fromDriver();
				foreach (string id : queriedIds)
				{
					int addIdx
					if (m_mAddOrUpdateBufferIndices.Find(id, addIdx))
					{
						bufferedResults.Insert(entities.Get(addIdx));
						continue;
					}

					fromDriver.Insert(id);
				}

				// all missing ids combine into new condition and send to driver
				if (!fromDriver.IsEmpty())
					processedCondition = EL_DbFind.Id().EqualsAnyOf(fromDriver);

				// -> pre results + further query (maybe)
			}
			else if (entities)
			{
				// do broad in memory search with this condition on add or update
				if (processedCondition)
					entities = EL_DbFindConditionEvaluator.GetFiltered(entities, processedCondition);

				// Add exclusion for known removed and alraedy retrieved results
				array<string> alreadyKnown();
				if (removedIds)
					alreadyKnown.Copy(removedIds);

				alreadyKnown.Reserve(alreadyKnown.Count() + entities.Count());

				foreach (EL_DbEntity entity : entities)
				{
					bufferedResults.Insert(entity);
					alreadyKnown.Insert(entity.GetId());
				}

				// send query to db with exlude for known update or removed ids
				if (!alreadyKnown.IsEmpty())
				{
					EL_DbFindCondition exclude = EL_DbFind.Id().Not().EqualsAnyOf(alreadyKnown);

					if (processedCondition)
					{
						processedCondition = EL_DbFind.And({processedCondition, exclude});
					}
					else
					{
						processedCondition = exclude;
					}
				}

				// -> pre results + further query
			}

			// no ordering to driver if there are any buffered results
			// combined results order in memory if there was orderby
			//if (!bufferedResults.IsEmpty())
			//	orderByDriver = null;


			// original search condition fully satisfied by buffered results, return early
			// ---- Also - If no order by and current results are enough for offset + limit return those first. >>> disabled because it fucks up result processing logic below
			if ((condition && !processedCondition)/* ||
				(!orderBy && (limit != -1) && ((Math.Max(offset, 0) + limit) <= bufferedResults.Count()))*//*)
			{
				if (orderBy)
					bufferedResults = EL_DbEntitySorter.GetSorted(bufferedResults, orderBy);

				array<ref EL_DbEntity> earlyResult();
				if (offset != -1 || limit != -1)
				{
					ApplyLimitOffset(bufferedResults, limit, offset, earlyResult);
				}
				else
				{
					earlyResult = bufferedResults;
				}

				if (async)
				{
					if (callback)
						callback.Invoke(EL_EDbOperationStatusCode.SUCCESS, earlyResult);

					return null;
				}
				else
				{
					return new EL_DbFindResultMultiple<EL_DbEntity>(EL_EDbOperationStatusCode.SUCCESS, earlyResult);
				}
			}

			/*
			// adjust offset and limit to account for buffered results and removals
			if (limit != -1)
				driverLimit += bufferedResults.Count();

			if (offset > 0)
			{
				driverOffset = Math.Max(0, offset - bufferedResults.Count());

				// offset: 3 -5 -> 0 limit:3 +5 -> 8 ---> 3 offset then 3 read
				// offset: 6 -5 -> 1 limit:3 +5 -> 8 ---> 5 offset then 3 read
				compenstatedOffset = Math.Min(offset, driverOffset);
			}
			*//*
		}

		if (async)
		{
			EL_DbBufferedFindAllAsyncContext context(this, bufferedResults, orderBy, limit, compenstatedOffset, callback);
			m_pDriver.FindAllAsync(entityType, processedCondition, orderByDriver, driverLimit, driverOffset, new EL_DbBufferedFindAllAsyncProcessorCallback(context: context));
			return null;
		}

		EL_DbFindResultMultiple<EL_DbEntity> result = m_pDriver.FindAll(entityType, processedCondition, orderByDriver, driverLimit, driverOffset);
		return ProcessResult(result.GetStatusCode(), result.GetEntities(), bufferedResults, orderBy, limit, compenstatedOffset);
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindResultMultiple<EL_DbEntity> ProcessResult(
		EL_EDbOperationStatusCode code,
		array<ref EL_DbEntity> findResults,
		array<ref EL_DbEntity> bufferedResults,
		array<ref TStringArray> orderBy,
		int limit,
		int offset)
	{
		//We need some strong ref for this, function argument won't keep it alive
		array<ref EL_DbEntity> processedResult = findResults;

		if (limit == -1)
			limit = int.MAX;

		int dbCount = 5;
		int bufferCount = bufferedResults.Count();
		int totalCount = dbCount + bufferCount;
		limit = Math.Min(totalCount - offset, limit);
		int needed = limit - findResults.Count();
		int startIndex = offset + findResults.Count() - dbCount;

		PrintFormat("Needs %1 starting at index %2 of buffer", needed, startIndex);
		// Problem: We do not know exactly which page an entry might have been ordered into on query.
		// Remembering for which page it was returned could lead to inaccurate results if the queries or data changes ...

		// append buffered results at the end to not disturb offset/limit while not using any ordering
		if (needed > 0)
		{
			processedResult.Reserve(processedResult.Count() + needed);
			for (int n = 0; n < needed; n++)
			{
				processedResult.Insert(bufferedResults.Get(startIndex + n));
			}
		}

		if (orderBy)
			processedResult = EL_DbEntitySorter.GetSorted(processedResult, orderBy);

		//if (processedResult.Count() > limit)
		//	processedResult.Resize(limit);

		/*
		// apply order + limit/offset
		if (limit != -1 || offset != -1)
		{
			array<ref EL_DbEntity> limitedResults();
			ApplyLimitOffset(processedResult, limit, offset, limitedResults);
			processedResult = limitedResults;
		}
		*//*

		return new EL_DbFindResultMultiple<EL_DbEntity>(code, processedResult);
	}

	//------------------------------------------------------------------------------------------------
	protected static void ApplyLimitOffset(array<ref EL_DbEntity> entities, int limit, int offset, array<ref EL_DbEntity> resultEntites)
	{
		foreach (int idx, EL_DbEntity entity : entities)
		{
			// Respect output limit is specified
			if (limit != -1 && resultEntites.Count() >= limit)
				break;

			// Skip the first n records if offset specified (for paginated loading together with limit)
			if (offset != -1 && idx < offset)
				continue;

			resultEntites.Insert(entity);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected EL_DbFindCondition ProcessCondition(
		typename entityType,
		EL_DbFindCondition condition,
		bool skipAddOrUpdateRemoval,
		array<string> queriedIds,
		out bool boolComplexCondition)
	{
		EL_DbFindFieldStringMultiple stringMultipleCondition = EL_DbFindFieldStringMultiple.Cast(condition);
		if (stringMultipleCondition)
		{
			if (stringMultipleCondition.m_sFieldPath != EL_DbEntity.FIELD_ID ||
				stringMultipleCondition.m_eComparisonOperator != EL_EDbFindOperator.EQUAL)
			{
				boolComplexCondition = true;
				return condition;
			}

			array<string> removedIds = m_mRemoveBufferIds.Get(entityType);
			array<string> updatedIds = m_mAddOrUpdateBufferIds.Get(entityType);

			array<int> remove();
			foreach (int idx, string id : stringMultipleCondition.m_aComparisonValues)
			{
				if (removedIds && removedIds.Contains(id))
				{
					remove.Insert(idx);
				}
				else if (!skipAddOrUpdateRemoval && updatedIds && updatedIds.Contains(id))
				{
					remove.Insert(idx);
					queriedIds.Insert(id);
				}
			}

			if (remove.Count() == stringMultipleCondition.m_aComparisonValues.Count())
				return null; // No additional conditions, all ids satifies by buffered info

			foreach (int removeIdx : remove)
			{
				stringMultipleCondition.m_aComparisonValues.RemoveOrdered(removeIdx);
			}

			return stringMultipleCondition;
		}

		EL_DbFindOr orCondition = EL_DbFindOr.Cast(condition);
		if (orCondition)
		{
			array<int> remove();
			foreach (int idx, EL_DbFindCondition childCondition : orCondition.m_Conditions)
			{
				EL_DbFindCondition processed = ProcessCondition(entityType, childCondition, skipAddOrUpdateRemoval, queriedIds, boolComplexCondition);
				if (!processed)
					remove.Insert(idx); // Condition consumed by pre-processing (e.g. removed/updated ids only)
			}

			if (remove.Count() == orCondition.m_Conditions.Count())
				return null; // all or conditions consumed

			foreach (int removeIdx : remove)
			{
				orCondition.m_Conditions.RemoveOrdered(removeIdx);
			}

			return orCondition;
		}

		EL_DbFindAnd andCondition = EL_DbFindAnd.Cast(condition);
		if (andCondition)
		{
			foreach (int idx, EL_DbFindCondition childCondition : andCondition.m_Conditions)
			{
				EL_DbFindCondition processed = ProcessCondition(entityType, childCondition, true, queriedIds, boolComplexCondition);
				if (!processed)
					return null; //Whole condition can not be met due to removed id
			}
		}

		return condition;
	}

	//------------------------------------------------------------------------------------------------
	array<ref EL_DbEntity> SyncResults(array<ref EL_DbEntity> entities)
	{
		array<int> remove();
		array<ref EL_DbEntity> updates;
		foreach (int idx, EL_DbEntity entity : entities)
		{
			if (!updates)
				updates = m_mAddOrUpdateBufferEntities.Get(entity.Type());

			string id = entity.GetId();
			if (m_mRemoveBufferIndices.Contains(id))
			{
				remove.Insert(idx);
				continue;
			}

			int updateIdx;
			if (m_mAddOrUpdateBufferIndices.Find(id, updateIdx))
				entities.Set(idx, updates.Get(updateIdx));
		}

		foreach (int idx : remove)
		{
			entities.RemoveOrdered(idx);
		}

		return entities;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbDriverBufferWrapper(notnull EL_DbDriver driver)
	{
		m_pDriver = driver;

		m_mRemoveBufferIndices = new map<string, int>();
		m_mRemoveBufferIds = new map<typename, ref array<string>>();
		m_mRemoveBufferAsync = new map<typename, ref array<bool>>();

		m_mAddOrUpdateBufferIndices = new map<string, int>();
		m_mAddOrUpdateBufferIds = new map<typename, ref array<string>>();
		m_mAddOrUpdateBufferAsync = new map<typename, ref array<bool>>();
		m_mAddOrUpdateBufferEntities = new map<typename, ref array<ref EL_DbEntity>>();
	}

	//------------------------------------------------------------------------------------------------
	void ~EL_DbDriverBufferWrapper()
	{
		Flush(forceBlocking: true);
	}
};

class EL_DbBufferedFindAllAsyncContext
{
	EL_DbDriverBufferWrapper m_pBufferWrapper;
	ref array<ref EL_DbEntity> m_aBufferedResults;
	ref array<ref TStringArray> m_aOrderBy;
	int m_iLimit;
	int m_iOffset;
	ref EL_DbFindCallbackBase m_pCallback;

	//------------------------------------------------------------------------------------------------
	void EL_DbBufferedFindAllAsyncContext(
		EL_DbDriverBufferWrapper bufferWrapper,
		array<ref EL_DbEntity> bufferedResults,
		array<ref TStringArray> orderBy,
		int limit,
		int offset,
		EL_DbFindCallbackBase callback)
	{
		m_pBufferWrapper = bufferWrapper;
		m_aBufferedResults = bufferedResults;
		m_aOrderBy = orderBy;
		m_iLimit = limit;
		m_iOffset = offset;
		m_pCallback = callback;
	}
};

class EL_DbBufferedFindAllAsyncProcessorCallback : EL_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		EL_DbBufferedFindAllAsyncContext context = EL_DbBufferedFindAllAsyncContext.Cast(m_pContext);
		EL_DbFindResultMultiple<EL_DbEntity> processedResult = context.m_pBufferWrapper.ProcessResult(
			code,
			findResults,
			context.m_aBufferedResults,
			context.m_aOrderBy,
			context.m_iLimit,
			context.m_iOffset);

		// for each buffered result, check current buffer if we can replace entity with that, to make sure we have the latest info, even if callback is sent, changes happen, callback invokes
		// check if they were just removed too
		array<ref EL_DbEntity> resultEntities = context.m_pBufferWrapper.SyncResults(processedResult.GetEntities());

		if (context.m_pCallback)
			context.m_pCallback.Invoke(processedResult.GetStatusCode(), resultEntities);
	}
};
*/
