class EL_PersistentScriptedStateLoaderTests : TestSuite
{
	ref EL_DbContext m_pPreviousContext;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
    void Setup()
    {
		// Change db context to in memory for this test suite
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		m_pPreviousContext = persistenceManager.GetDbContext();
		persistenceManager.SetDbContext(EL_DbContextFactory.GetContext("testing", false));
    }

	//------------------------------------------------------------------------------------------------
    [Step(EStage.TearDown)]
    void TearDown()
    {
		EL_PersistenceManagerInternal.GetInternalInstance().SetDbContext(m_pPreviousContext);
		m_pPreviousContext = null;
    }
}

[EL_PersistentScriptedStateSettings(EL_Test_ScriptedStateLoaderDummy, EL_Test_ScriptedStateLoaderDummySaveData, selfDelete: true)]
class EL_Test_ScriptedStateLoaderDummy : EL_PersistentScriptedStateBase
{
	int m_iIntValue;

	//------------------------------------------------------------------------------------------------
	static EL_Test_ScriptedStateLoaderDummy Create(int intValue)
	{
		EL_Test_ScriptedStateLoaderDummy instance();
		instance.m_iIntValue = intValue;
		return instance;
	}
}

[EL_DbName(EL_Test_ScriptedStateLoaderDummySaveData, "ScriptedStateLoaderDummy")]
class EL_Test_ScriptedStateLoaderDummySaveData : EL_ScriptedStateSaveDataBase
{
	int m_iIntValue;
}

[Test("EL_PersistentScriptedStateLoaderTests", 3)]
class EL_Test_PersistentScriptedStateLoader_GetSingleton_NotExisting_Created : TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EL_Test_ScriptedStateLoaderDummy instance = EL_PersistentScriptedStateLoader<EL_Test_ScriptedStateLoaderDummy>.LoadSingleton();

		// Assert
		SetResult(new EL_TestResult(instance && instance.GetPersistentId()));
	}
}

[Test("EL_PersistentScriptedStateLoaderTests", 3)]
class EL_Test_PersistentScriptedStateLoader_GetSingleton_Existing_Returned : TestBase
{
	ref EL_Test_ScriptedStateLoaderDummy m_pExisting;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		m_pExisting = EL_Test_ScriptedStateLoaderDummy.Create(1001);
		m_pExisting.Save();
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EL_Test_ScriptedStateLoaderDummy instance = EL_PersistentScriptedStateLoader<EL_Test_ScriptedStateLoaderDummy>.LoadSingleton();

		// Assert
		SetResult(new EL_TestResult(
			instance &&
			instance.GetPersistentId() == m_pExisting.GetPersistentId() &&
			instance.m_iIntValue == m_pExisting.m_iIntValue));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		m_pExisting = null;
	}
}

[Test("EL_PersistentScriptedStateLoaderTests", 3)]
class EL_Test_PersistentScriptedStateLoader_GetSingletonAsync_NotExisting_Created : TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Act()
	{
		EL_ScriptedStateLoaderCallbackSingle<EL_Test_ScriptedStateLoaderDummy> callback();
		callback.ConfigureInvoker(this, "Assert");
		EL_PersistentScriptedStateLoader<EL_Test_ScriptedStateLoaderDummy>.LoadSingletonAsync(callback);
	}

	//------------------------------------------------------------------------------------------------
	void Assert(Managed context, EL_Test_ScriptedStateLoaderDummy instance)
	{
		SetResult(new EL_TestResult(instance && instance.GetPersistentId()));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	bool AwaitResult()
	{
		return GetResult();
	}
}

[Test("EL_PersistentScriptedStateLoaderTests", 3)]
class EL_Test_PersistentScriptedStateLoader_GetSingletonAsync_Existing_Returned : TestBase
{
	ref EL_Test_ScriptedStateLoaderDummy m_pExisting;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		m_pExisting = EL_Test_ScriptedStateLoaderDummy.Create(1002);
		m_pExisting.Save();
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Act()
	{
		EL_ScriptedStateLoaderCallbackSingle<EL_Test_ScriptedStateLoaderDummy> callback();
		callback.ConfigureInvoker(this, "Assert");
		EL_PersistentScriptedStateLoader<EL_Test_ScriptedStateLoaderDummy>.LoadSingletonAsync(callback);
	}

	//------------------------------------------------------------------------------------------------
	void Assert(Managed context, EL_Test_ScriptedStateLoaderDummy instance)
	{
		SetResult(new EL_TestResult(
			instance &&
			instance.GetPersistentId() == m_pExisting.GetPersistentId() &&
			instance.m_iIntValue == m_pExisting.m_iIntValue));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	bool AwaitResult()
	{
		return GetResult();
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		m_pExisting = null;
	}
}

[Test("EL_PersistentScriptedStateLoaderTests", 3)]
class EL_Test_PersistentScriptedStateLoader_GetAsync_Existing_Returned : TestBase
{
	ref EL_Test_ScriptedStateLoaderDummy m_pExisting;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		m_pExisting = EL_Test_ScriptedStateLoaderDummy.Create(1003);
		m_pExisting.Save();
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Act()
	{
		EL_ScriptedStateLoaderCallbackSingle<EL_Test_ScriptedStateLoaderDummy> callback();
		callback.ConfigureInvoker(this, "Assert");
		EL_PersistentScriptedStateLoader<EL_Test_ScriptedStateLoaderDummy>.LoadAsync(m_pExisting.GetPersistentId(), callback);
	}

	//------------------------------------------------------------------------------------------------
	void Assert(Managed context, EL_Test_ScriptedStateLoaderDummy instance)
	{
		SetResult(new EL_TestResult(
			instance &&
			instance.GetPersistentId() == m_pExisting.GetPersistentId() &&
			instance.m_iIntValue == m_pExisting.m_iIntValue));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	bool AwaitResult()
	{
		return GetResult();
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		m_pExisting = null;
	}
}

[Test("EL_PersistentScriptedStateLoaderTests", 3)]
class EL_Test_PersistentScriptedStateLoader_GetAsync_MultipleExisting_AllReturned : TestBase
{
	ref EL_Test_ScriptedStateLoaderDummy m_pExisting1;
	ref EL_Test_ScriptedStateLoaderDummy m_pExisting2;
	ref array<string> m_aIds;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		m_pExisting1 = EL_Test_ScriptedStateLoaderDummy.Create(1004);
		m_pExisting2 = EL_Test_ScriptedStateLoaderDummy.Create(1005);
		m_pExisting1.Save();
		m_pExisting2.Save();
		m_aIds = {m_pExisting1.GetPersistentId(), m_pExisting2.GetPersistentId()};
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Act()
	{
		EL_ScriptedStateLoaderCallbackMultiple<EL_Test_ScriptedStateLoaderDummy> callback();
		callback.ConfigureInvoker(this, "Assert");
		EL_PersistentScriptedStateLoader<EL_Test_ScriptedStateLoaderDummy>.LoadAsync(m_aIds, callback);
	}

	//------------------------------------------------------------------------------------------------
	void Assert(Managed context, array<ref EL_Test_ScriptedStateLoaderDummy> instances)
	{
		SetResult(new EL_TestResult(
			instances &&
			instances.Count() == 2 &&
			m_aIds.Contains(instances.Get(0).GetPersistentId()) &&
			m_aIds.Contains(instances.Get(1).GetPersistentId())));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	bool AwaitResult()
	{
		return GetResult();
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		m_pExisting1 = null;
		m_pExisting2 = null;
	}
}
