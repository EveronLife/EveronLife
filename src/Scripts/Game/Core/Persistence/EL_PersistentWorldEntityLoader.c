class EL_PersistentWorldEntityLoader
{
	protected static ref map<string, typename> s_mSaveDataTypeCache;

	//------------------------------------------------------------------------------------------------
	//! Load and spawn an entity by save-data type and persistent id
	//! \param saveDataType save-data type of the entity
	//! \param persistentId Persistent id
	//! \return Spawned entity or null on failure
	static IEntity Load(typename saveDataType, string persistentId)
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, EL_DbFind.Id().Equals(persistentId), limit: 1).GetEntities();
		if (!findResults || findResults.Count() != 1) return null;
		return persistenceManager.SpawnWorldEntity(EL_EntitySaveData.Cast(findResults.Get(0)));
	}

	//------------------------------------------------------------------------------------------------
	//! Load and spawn an entity by prefab and persistent id
	//! \param prefab Prefab resource name
	//! \param persistentId Persistent id
	//! \return Spawned entity or null on failure
	static IEntity Load(string prefab, string persistentId)
	{
		typename type = GetSaveDataType(prefab); // Can not be inlined because of typename crash bug
		return Load(type, persistentId);
	}

	//------------------------------------------------------------------------------------------------
	//! see Load(typename, string)
	static void LoadAsync(typename saveDataType, string persistentId, EL_DataCallbackSingle<IEntity> callback = null)
	{
		EL_WorldEntityLoaderProcessorCallbackSingle processorCallback();
		processorCallback.Setup(callback);
		EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, EL_DbFind.Id().Equals(persistentId), limit: 1, callback: processorCallback);
	}

	//------------------------------------------------------------------------------------------------
	//! see Load(string, string)
	static void LoadAsync(string prefab, string persistentId, EL_DataCallbackSingle<IEntity> callback = null)
	{
		typename type = GetSaveDataType(prefab); // Can not be inlined because of typename crash bug
		LoadAsync(type, persistentId, callback);
	}

	//------------------------------------------------------------------------------------------------
	//! Load and spawn multiple entities by save-data type and persistent ids / or all if ids is null/empty
	//! \param saveDataType save-data type of the entities
	//! \param persistentId Array of persistent ids - or null/empty for load all.
	//! \return array of entities if spawned successfully, else empty
	static array<IEntity> Load(typename saveDataType, array<string> persistentIds = null)
	{
		array<IEntity> resultEntities();

		EL_DbFindCondition condition;
		if (persistentIds && !persistentIds.IsEmpty())
		{
			condition = EL_DbFind.Id().EqualsAnyOf(persistentIds);
		}

		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		array<ref EL_DbEntity> findResults = persistenceManager.GetDbContext().FindAll(saveDataType, condition).GetEntities();
		if (findResults)
		{
			foreach (EL_DbEntity findResult : findResults)
			{
				IEntity entity = persistenceManager.SpawnWorldEntity(EL_EntitySaveData.Cast(findResult));
				if (entity) resultEntities.Insert(entity);
			}
		}

		return resultEntities;
	}

	//------------------------------------------------------------------------------------------------
	//! Load and spawn multiple entities by prefab and persistent ids / or all if ids is null/empty
	//! \param Prefab resource name of the entities
	//! \param persistentId Array of persistent ids - or null/empty for load all.
	//! \return array of entities if spawned successfully, else empty
	static array<IEntity> Load(string prefab, array<string> persistentIds = null)
	{
		typename type = GetSaveDataType(prefab); // Can not be inlined because of typename crash bug
		return Load(type, persistentIds);
	}

	//------------------------------------------------------------------------------------------------
	//! see Load(typename, array<string>)
	static void LoadAsync(typename saveDataType, array<string> persistentIds = null, EL_DataCallbackMultiple<IEntity> callback = null)
	{
		EL_WorldEntityLoaderProcessorCallbackMultiple processorCallback();
		processorCallback.Setup(callback);

		EL_DbFindCondition condition;
		if (persistentIds && !persistentIds.IsEmpty())
		{
			condition = EL_DbFind.Id().EqualsAnyOf(persistentIds);
		}

		EL_PersistenceManager.GetInstance().GetDbContext().FindAllAsync(saveDataType, condition, callback: processorCallback);
	}

	//------------------------------------------------------------------------------------------------
	//! see Load(string, array<string>)
	static void LoadAsync(string prefab, array<string> persistentIds = null, EL_DataCallbackMultiple<IEntity> callback = null)
	{
		typename type = GetSaveDataType(prefab); // Can not be inlined because of typename crash bug
		LoadAsync(type, persistentIds, callback);
	}

	//------------------------------------------------------------------------------------------------
	protected static typename GetSaveDataType(string prefab)
	{
		if (!s_mSaveDataTypeCache) s_mSaveDataTypeCache = new map<string, typename>();

		typename resultType = s_mSaveDataTypeCache.Get(prefab);

		if (!resultType)
		{
			Resource resource = Resource.Load(prefab);
			if (resource && resource.IsValid())
			{
				IEntitySource entitySource = resource.GetResource().ToEntitySource();
				for (int nComponentSource = 0, count = entitySource.GetComponentCount(); nComponentSource < count; nComponentSource++)
				{
					IEntityComponentSource componentSource = entitySource.GetComponent(nComponentSource);
					typename componentType = componentSource.GetClassName().ToType();
					if (componentType.IsInherited(EL_PersistenceComponent))
					{
						BaseContainer saveDataContainer = componentSource.GetObject("m_pSaveData");
						if (saveDataContainer)
						{
							resultType = EL_Utils.TrimEnd(saveDataContainer.GetClassName(), 5).ToType();
							break;
						}
					}
				}
			}
		}

		s_mSaveDataTypeCache.Set(prefab, resultType);

		return resultType;
	}
};

class EL_WorldEntityLoaderProcessorCallbackSingle : EL_DbFindCallbackSingle<EL_EntitySaveData>
{
	ref EL_DataCallbackSingle<IEntity> m_pOuterCallback;

	//------------------------------------------------------------------------------------------------
	override void OnSuccess(EL_EntitySaveData resultData, Managed context)
	{
		IEntity resultWorldEntity = EL_PersistenceManager.GetInstance().SpawnWorldEntity(resultData);
		if (m_pOuterCallback) 
			m_pOuterCallback.Invoke(resultWorldEntity);
	}

	//------------------------------------------------------------------------------------------------
	override void OnFailure(EL_EDbOperationStatusCode resultCode, Managed context)
	{
		if (m_pOuterCallback) 
			m_pOuterCallback.Invoke(null);
	}

	//------------------------------------------------------------------------------------------------
	void Setup(EL_DataCallbackSingle<IEntity> outerCallback)
	{
		m_pOuterCallback = outerCallback;
	}
};

class EL_WorldEntityLoaderProcessorCallbackMultiple : EL_DbFindCallbackMultiple<EL_EntitySaveData>
{
	ref EL_DataCallbackMultiple<IEntity> m_pOuterCallback;

	//------------------------------------------------------------------------------------------------
	override void OnSuccess(array<ref EL_EntitySaveData> resultData, Managed context)
	{
		array<IEntity> resultEntities();

		if (resultData)
		{
			EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();

			foreach (EL_EntitySaveData saveData : resultData)
			{
				IEntity entity = persistenceManager.SpawnWorldEntity(saveData);
				if (entity) resultEntities.Insert(entity);
			}
		}

		if (m_pOuterCallback) 
			m_pOuterCallback.Invoke(resultEntities);
	}

	//------------------------------------------------------------------------------------------------
	override void OnFailure(EL_EDbOperationStatusCode resultCode, Managed context)
	{
		if (m_pOuterCallback) 
			m_pOuterCallback.Invoke(new array<IEntity>());
	}

	//------------------------------------------------------------------------------------------------
	void Setup(EL_DataCallbackMultiple<IEntity> outerCallback)
	{
		m_pOuterCallback = outerCallback;
	}
};
