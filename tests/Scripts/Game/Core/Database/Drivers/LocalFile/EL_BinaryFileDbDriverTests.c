class EL_BinaryFileDbDriverTests : TestSuite
{
	static const string DB_NAME = "BinaryFileDbDriverTests";

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
	static void WriteEntity(string dbDir, EL_Test_BinFileDbDriverEntity entity)
	{
		SCR_BinSaveContext writer();
		writer.WriteValue("data", entity);
		FileIO.MakeDirectory(dbDir);
		writer.SaveToFile(string.Format("%1/%2.bin", dbDir, entity.GetId()));
	}

	//------------------------------------------------------------------------------------------------
	static void DeleteEntity(string dbDir, string entityId)
	{
		FileIO.DeleteFile(string.Format("%1/%2.bin", dbDir, entityId));
	}
}

class EL_Test_BinFileDbDriverEntity : EL_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	void EL_Test_BinFileDbDriverEntity(string id, float floatValue, string stringValue)
	{
		SetId(id);
		m_fFloatValue = floatValue;
		m_sStringValue = stringValue;
	}
}

class EL_Test_BinFileDbDriverEntityr_TestBase : TestBase
{
	ref EL_JsonFileDbDriver driver;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Construct()
	{
		driver = new EL_JsonFileDbDriver();
	}
}

[Test("EL_BinaryFileDbDriverTests")]
class EL_Test_BinaryFileDbDriver_AddOrUpdate_NewEntity_ReadFromFileSuccessfully : EL_Test_BinFileDbDriverEntityr_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		driver.Initalize(string.Format("%1?cache=false", EL_BinaryFileDbDriverTests.DB_NAME));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EL_Test_BinFileDbDriverEntity entity("TEST0000-0000-0001-0000-000000000001", 42.42, "Hello World");

		driver.AddOrUpdate(entity);

		EL_EDbOperationStatusCode resultCode = driver.AddOrUpdate(entity);

		// Assert
		if (resultCode != EL_EDbOperationStatusCode.SUCCESS)
		{
			SetResult(new EL_TestResult(false));
			return;
		}

		array<ref EL_DbEntity> results = driver.FindAll(EL_Test_BinFileDbDriverEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();

		if (results.Count() != 1)
		{
			SetResult(new EL_TestResult(false));
			return;
		}

		EL_Test_BinFileDbDriverEntity resultEntity = EL_Test_BinFileDbDriverEntity.Cast(results.Get(0));

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
		EL_BinaryFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EL_Test_BinFileDbDriverEntity), "TEST0000-0000-0001-0000-000000000001");
	}
}
