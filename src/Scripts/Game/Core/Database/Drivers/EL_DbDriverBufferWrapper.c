class EL_DbDriverBufferWrapper : EL_DbDriver
{
	protected ref EL_DbDriver m_pDriver;
	protected ref map<string, ref EL_DbDriverBufferRemoveEntry> m_aRemoveBuffer;
	protected ref map<string, ref EL_DbDriverBufferAddOrUpdateEntry> m_mAddOrUpdateBuffer;
	protected bool m_bFlushActive;

	//------------------------------------------------------------------------------------------------
	override EL_EDbOperationStatusCode AddOrUpdate(notnull EL_DbEntity entity)
	{
		string id = entity.GetId();
		m_aRemoveBuffer.Remove(id);
		m_mAddOrUpdateBuffer.Set(id, new EL_DbDriverBufferAddOrUpdateEntry(entity, false));
		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		m_mAddOrUpdateBuffer.Remove(entityId);
		m_aRemoveBuffer.Set(entityId, new EL_DbDriverBufferRemoveEntry(entityType, false));
		return EL_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EL_DbFindResultMultiple<EL_DbEntity> FindAll(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		EL_DbFindResultMultiple<EL_DbEntity> result = m_pDriver.FindAll(entityType, condition, orderBy, limit, offset);
		return new EL_DbFindResultMultiple<EL_DbEntity>(result.GetStatusCode(), ProcessResults(result.GetEntities()));
	}

	//------------------------------------------------------------------------------------------------
	override void AddOrUpdateAsync(notnull EL_DbEntity entity, EL_DbOperationStatusOnlyCallback callback = null)
	{
		string id = entity.GetId();
		m_aRemoveBuffer.Remove(id);
		m_mAddOrUpdateBuffer.Set(id, new EL_DbDriverBufferAddOrUpdateEntry(entity, true));
		if (callback)
			callback.Invoke(EL_EDbOperationStatusCode.SUCCESS);
	}

	//------------------------------------------------------------------------------------------------
	override void RemoveAsync(typename entityType, string entityId, EL_DbOperationStatusOnlyCallback callback = null)
	{
		m_mAddOrUpdateBuffer.Remove(entityId);
		m_aRemoveBuffer.Set(entityId, new EL_DbDriverBufferRemoveEntry(entityType, true));
		if (callback)
			callback.Invoke(EL_EDbOperationStatusCode.SUCCESS);
	}

	//------------------------------------------------------------------------------------------------
	override void FindAllAsync(typename entityType, EL_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EL_DbFindCallbackBase callback = null)
	{
		EL_DbBufferedFindAllAsyncContext context(this, callback);
		m_pDriver.FindAllAsync(entityType, condition, orderBy, limit, offset, new EL_DbBufferedFindAllAsyncProcessorCallback(context: context));
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

		foreach (string id, EL_DbDriverBufferRemoveEntry entry : m_aRemoveBuffer)
		{
			if (entry.m_bAsync)
			{
				m_pDriver.RemoveAsync(entry.m_tSaveDataType, id);
			}
			else
			{
				m_pDriver.Remove(entry.m_tSaveDataType, id);
			}

			if ((++operation % maxBatchSize == 0) && !forceBlocking)
				Sleep(100);
		}

		m_aRemoveBuffer.Clear();

		foreach (string id, EL_DbDriverBufferAddOrUpdateEntry entry : m_mAddOrUpdateBuffer)
		{
			if (entry.m_bAsync)
			{
				m_pDriver.AddOrUpdateAsync(entry.m_pEntity);
			}
			else
			{
				m_pDriver.AddOrUpdate(entry.m_pEntity);
			}

			if ((++operation % maxBatchSize == 0) && !forceBlocking)
				Sleep(100);
		}

		m_mAddOrUpdateBuffer.Clear();

		m_bFlushActive = false;
	}

	//------------------------------------------------------------------------------------------------
	array<ref EL_DbEntity> ProcessResults(array<ref EL_DbEntity> entities)
	{
		array<int> remove();

		foreach (int idx, EL_DbEntity entity : entities)
		{
			string id = entity.GetId();
			if (m_aRemoveBuffer.Contains(id))
			{
				remove.Insert(idx);
				continue;
			}

			EL_DbDriverBufferAddOrUpdateEntry bufferdData = m_mAddOrUpdateBuffer.Get(id);
			if (bufferdData)
				entities.Set(idx, bufferdData.m_pEntity);
		}

		foreach (int idx : remove)
		{
			entities.Remove(idx);
		}

		return entities;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbDriverBufferWrapper(notnull EL_DbDriver driver)
	{
		m_pDriver = driver;
		m_aRemoveBuffer = new map<string, ref EL_DbDriverBufferRemoveEntry>();
		m_mAddOrUpdateBuffer = new map<string, ref EL_DbDriverBufferAddOrUpdateEntry>();
	}

	//------------------------------------------------------------------------------------------------
	void ~EL_DbDriverBufferWrapper()
	{
		Flush(forceBlocking: true);
	}
};

class EL_DbDriverBufferRemoveEntry
{
	typename m_tSaveDataType;
	bool m_bAsync;

	//------------------------------------------------------------------------------------------------
	void EL_DbDriverBufferRemoveEntry(typename saveDataType, bool async)
	{
		m_tSaveDataType = saveDataType;
		m_bAsync = async;
	}
};

class EL_DbDriverBufferAddOrUpdateEntry
{
	ref EL_DbEntity m_pEntity;
	bool m_bAsync;

	//------------------------------------------------------------------------------------------------
	void EL_DbDriverBufferAddOrUpdateEntry(EL_DbEntity entity, bool async)
	{
		m_pEntity = entity;
		m_bAsync = async;
	}
};

class EL_DbBufferedFindAllAsyncContext
{
	EL_DbDriverBufferWrapper m_pBufferWrapper;
	ref EL_DbFindCallbackBase m_pCallback;

	//------------------------------------------------------------------------------------------------
	void EL_DbBufferedFindAllAsyncContext(EL_DbDriverBufferWrapper bufferWrapper, EL_DbFindCallbackBase callback)
	{
		m_pBufferWrapper = bufferWrapper;
		m_pCallback = callback;
	}
};

class EL_DbBufferedFindAllAsyncProcessorCallback : EL_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EL_EDbOperationStatusCode code, array<ref EL_DbEntity> findResults)
	{
		auto context = EL_DbBufferedFindAllAsyncContext.Cast(m_pContext);
		if (context.m_pCallback)
			context.m_pCallback.Invoke(code, context.m_pBufferWrapper.ProcessResults(findResults));
	}
};
