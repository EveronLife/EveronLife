class EL_JsonFileDbDriverTests : TestSuite
{
	static const string DB_NAME = "JsonFileDbDriverTests";

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
    void Setup()
    {
    }

	//------------------------------------------------------------------------------------------------
    [Step(EStage.TearDown)]
    void TearDown()
    {
		string dir = string.Format("%1/%2", EL_FileDbDriverBase.DB_BASE_DIR, DB_NAME);

		array<string> paths = {};
		System.FindFiles(paths.Insert, dir, "");

		foreach (string path : paths)
		{
			FileIO.DeleteFile(path);
		}

		FileIO.DeleteFile(dir);
    }

	//------------------------------------------------------------------------------------------------
	static void WriteEntity(string dbDir, EL_Test_JsonFileDbDriverEntity entity)
	{
		SCR_JsonSaveContext writer(false);
		writer.WriteValue("data", entity);
		FileIO.MakeDirectory(dbDir);
		writer.SaveToFile(string.Format("%1/%2.json", dbDir, entity.GetId()));
	}

	//------------------------------------------------------------------------------------------------
	static void DeleteEntity(string dbDir, string entityId)
	{
		FileIO.DeleteFile(string.Format("%1/%2.json", dbDir, entityId));
	}
}

class EL_Test_JsonFileDbDriverEntity : EL_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	void EL_Test_JsonFileDbDriverEntity(string id, float floatValue, string stringValue)
	{
		SetId(id);
		m_fFloatValue = floatValue;
		m_sStringValue = stringValue;
	}
}

class EL_Test_JsonFileDbDriver_TestBase : TestBase
{
	ref EL_JsonFileDbDriver driver;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Construct()
	{
		driver = new EL_JsonFileDbDriver();
	}
}

[Test("EL_JsonFileDbDriverTests")]
class EL_Test_JsonFileDbDriver_AddOrUpdate_NewEntity_ReadFromFileSuccessfully : EL_Test_JsonFileDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		driver.Initalize(string.Format("%1?cache=false", EL_JsonFileDbDriverTests.DB_NAME));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EL_Test_JsonFileDbDriverEntity entity("TEST0000-0000-0001-0000-000000000001", 42.42, "Hello World");

		driver.AddOrUpdate(entity);

		EL_EDbOperationStatusCode resultCode = driver.AddOrUpdate(entity);

		// Assert
		if (resultCode != EL_EDbOperationStatusCode.SUCCESS)
		{
			SetResult(new EL_TestResult(false));
			return;
		}

		array<ref EL_DbEntity> results = driver.FindAll(EL_Test_JsonFileDbDriverEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();

		if (results.Count() != 1)
		{
			SetResult(new EL_TestResult(false));
			return;
		}

		EL_Test_JsonFileDbDriverEntity resultEntity = EL_Test_JsonFileDbDriverEntity.Cast(results.Get(0));

		if (!resultEntity)
		{
			SetResult(new EL_TestResult(false));
			return;
		}

		SetResult(new EL_TestResult(
			resultEntity.GetId() == entity.GetId() &&
			resultEntity.m_fFloatValue == entity.m_fFloatValue &&
			resultEntity.m_sStringValue == entity.m_sStringValue));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000000001");
	}
}

[Test("EL_JsonFileDbDriverTests")]
class EL_Test_JsonFileDbDriver_Remove_ExistingEntity_FileDeleted : EL_Test_JsonFileDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		driver.Initalize(string.Format("%1?cache=false", EL_JsonFileDbDriverTests.DB_NAME));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EL_Test_JsonFileDbDriverEntity entity("TEST0000-0000-0001-0000-000000000002", 42.42, "Hello World");

		driver.AddOrUpdate(entity);

		EL_EDbOperationStatusCode resultCode = driver.Remove(EL_Test_JsonFileDbDriverEntity, entity.GetId());

		// Assert
		if (resultCode != EL_EDbOperationStatusCode.SUCCESS)
		{
			SetResult(new EL_TestResult(false));
			return;
		}

		string file = string.Format("%1/%2.json", driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), entity.GetId());
		SetResult(new EL_TestResult(!FileIO.FileExist(file)));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000000002");
	}
}

[Test("EL_JsonFileDbDriverTests")]
class EL_Test_JsonFileDbDriver_FindAll_IdOnly_ExactLoadAndCache : EL_Test_JsonFileDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		driver.Initalize(string.Format("%1?cache=true", EL_JsonFileDbDriverTests.DB_NAME));

		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001001", 41.1, "Existing 1001"));
		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001002", 41.2, "Existing 1002"));
		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001003", 41.3, "Existing 1003"));
		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001004", 41.4, "Existing 1004"));
		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001005", 41.5, "Existing 1005"));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EL_DbFindCondition condition = EL_DbFind.Id().EqualsAnyOf(EL_DbValues<string>.From({"TEST0000-0000-0001-0000-000000001001", "TEST0000-0000-0001-0000-000000001005"}));

		// Act
		array<ref EL_DbEntity> results = driver.FindAll(EL_Test_JsonFileDbDriverEntity, condition).GetEntities();

		// Assert
		SetResult(new EL_TestResult((results.Count() == 2) && (driver._GetEntityCache().m_EntityInstances.Count() == 2)));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001001");
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001002");
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001003");
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001004");
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001005");
	}
}

[Test("EL_JsonFileDbDriverTests")]
class EL_Test_JsonFileDbDriver_FindAll_ContentField_AllLoadedAndCached : EL_Test_JsonFileDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		driver.Initalize(string.Format("%1?cache=true", EL_JsonFileDbDriverTests.DB_NAME));

		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002001", 42.1, "Existing 2001"));
		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002002", 42.2, "Existing 2002"));
		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002003", 42.3, "Existing 2003"));
		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002004", 42.4, "Existing 2004"));
		EL_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), new EL_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002005", 42.5, "Existing 2005"));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EL_DbFindCondition condition = EL_DbFind.Or({
			EL_DbFind.Field("m_sStringValue").Contains("2001"),
			EL_DbFind.Field("m_fFloatValue").GreaterThanOrEqual(42.5)
		});

		array<ref EL_DbEntity> results = driver.FindAll(EL_Test_JsonFileDbDriverEntity, condition).GetEntities();

		// Assert
		SetResult(new EL_TestResult((results.Count() == 2) && (driver._GetEntityCache().m_EntityInstances.Count() == 5)));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002001");
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002002");
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002003");
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002004");
		EL_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002005");
	}
}
