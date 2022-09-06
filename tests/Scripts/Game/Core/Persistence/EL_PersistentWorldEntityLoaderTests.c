class EL_PersistentWorldEntityLoaderTests : TestSuite
{
	ref EL_DbContext m_pPreviousContext;
	
	[Step(EStage.Setup)]
    void Setup()
    {
		// Change db context to in memory for this test suite
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		m_pPreviousContext = persistenceManager.GetDbContext();
		persistenceManager.SetDbContext(EL_DbContextFactory.GetContext("testing", false));
    }

    [Step(EStage.TearDown)]
    void TearDown()
    {
		EL_PersistenceManagerInternal.GetInternalInstance().SetDbContext(m_pPreviousContext);
		m_pPreviousContext = null;
    }
}

class PersistentWorldEntityLoaderBase : TestBase
{
	EL_PersistenceComponent m_pExisting;
	
	[Step(EStage.Setup)]
	void Arrange()
	{
		m_pExisting = SpawnDummy();
	}
	
	EL_PersistenceComponent SpawnDummy()
	{
		EL_PersistenceComponent persistenceComponent;
		IEntity dummy = EL_Utils.SpawnEntityPrefab("{C95E11C60810F432}Prefabs/Items/Core/Item_Base.et", "0 0 0");
		if (dummy)
		{
			persistenceComponent = EL_PersistenceComponent.Cast(dummy.FindComponent(EL_PersistenceComponent));
			if (persistenceComponent) persistenceComponent.Save();
		}
		return persistenceComponent;
	}
	
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_pExisting.GetOwner());
		m_pExisting = null;
	}
}

[Test("EL_PersistentWorldEntityLoaderTests", 3)]
class EL_Test_PersistentWorldEntityLoader_Get_Existing_Spawned : PersistentWorldEntityLoaderBase
{ 
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		IEntity worldEntity = EL_PersistentWorldEntityLoader.Load(EL_Utils.GetPrefabName(m_pExisting.GetOwner()), m_pExisting.GetPersistentId());
		
		// Assert
		SetResult(new EL_TestResult(
			worldEntity &&
			m_pExisting.GetPersistentId() && 
			EL_PersistenceComponent.GetPersistentId(worldEntity) == m_pExisting.GetPersistentId()));
		
		// Cleanup
		SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
	}
}

[Test("EL_PersistentWorldEntityLoaderTests", 3)]
class EL_Test_PersistentWorldEntityLoader_GetAsync_Existing_Spawned : PersistentWorldEntityLoaderBase
{ 
	[Step(EStage.Main)]
	void Act()
	{		
		EL_WorldEntityLoaderCallbackSingle callback();
		callback.ConfigureInvoker(this, "Assert");
		EL_PersistentWorldEntityLoader.LoadAsync(EL_ItemSaveData, m_pExisting.GetPersistentId(), callback);
	}
	
	void Assert(Managed context, IEntity worldEntity)
	{		
		SetResult(new EL_TestResult(
			worldEntity &&
			m_pExisting.GetPersistentId() && 
			EL_PersistenceComponent.GetPersistentId(worldEntity) == m_pExisting.GetPersistentId()));
		
		// Cleanup
		SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
	}
	
	[Step(EStage.Main)]
	bool AwaitResult() 
	{
		return GetResult();
	}
}

[Test("EL_PersistentWorldEntityLoaderTests", 3)]
class EL_Test_PersistentWorldEntityLoader_Get_MultieExisting_AllSpawned : PersistentWorldEntityLoaderBase
{ 
	EL_PersistenceComponent m_pExisting2;
	ref array<string> m_aIds;
	
	override void Arrange()
	{
		super.Arrange();
		m_pExisting2 = SpawnDummy();
		m_aIds = {m_pExisting.GetPersistentId(), m_pExisting2.GetPersistentId()};
	}
	
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		array<IEntity> worldEntities = EL_PersistentWorldEntityLoader.Load(EL_ItemSaveData, m_aIds);
		
		// Assert
		SetResult(new EL_TestResult(
			worldEntities &&
			worldEntities.Count() == 2 &&
			m_aIds.Contains(EL_PersistenceComponent.GetPersistentId(worldEntities.Get(0))) &&
			m_aIds.Contains(EL_PersistenceComponent.GetPersistentId(worldEntities.Get(1)))));
		
		// Cleanup
		RemoveEntities(worldEntities);
	}
	
	void RemoveEntities(array<IEntity> worldEntities)
	{
		if (!worldEntities) return;
		foreach (IEntity worldEntity : worldEntities)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(worldEntity);
		} 
	}
	
	override void Cleanup()
	{
		super.Cleanup();
		
		SCR_EntityHelper.DeleteEntityAndChildren(m_pExisting2.GetOwner());
		m_pExisting2 = null;
	}
}

[Test("EL_PersistentWorldEntityLoaderTests", 3)]
class EL_Test_PersistentWorldEntityLoader_GetAsync_MultieExisting_AllSpawned : EL_Test_PersistentWorldEntityLoader_Get_MultieExisting_AllSpawned
{ 
	[Step(EStage.Main)]
	void Act()
	{		
		EL_WorldEntityLoaderCallbackMultiple callback();
		callback.ConfigureInvoker(this, "Assert");
		EL_PersistentWorldEntityLoader.LoadAsync(EL_ItemSaveData, m_aIds, callback);
	}
	
	void Assert(Managed context, array<IEntity> worldEntities)
	{		
		SetResult(new EL_TestResult(
			worldEntities &&
			worldEntities.Count() == 2 &&
			m_aIds.Contains(EL_PersistenceComponent.GetPersistentId(worldEntities.Get(0))) &&
			m_aIds.Contains(EL_PersistenceComponent.GetPersistentId(worldEntities.Get(1)))));
		
		// Cleanup
		RemoveEntities(worldEntities);
	}
	
	[Step(EStage.Main)]
	bool AwaitResult() 
	{
		return GetResult();
	}
}
