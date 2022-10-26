class EL_InMemoryDbDriverTests : TestSuite
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
    void Setup()
    {
    }

	//------------------------------------------------------------------------------------------------
    [Step(EStage.TearDown)]
    void TearDown()
    {
    }
}

class EL_Test_InMemoryDbDriverEntity : EL_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	void EL_Test_InMemoryDbDriverEntity(string id, float floatValue, string stringValue)
	{
		SetId(id);
		m_fFloatValue = floatValue;
		m_sStringValue = stringValue;
	}
}

//------------------------------------------------------------------------------------------------
[Test("EL_InMemoryDbDriverTests")]
TestResultBase EL_Test_InMemoryDbDriver_AddOrUpdate_NewEntity_Inserted()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	driver.Initalize("testing");

	EL_Test_InMemoryDbDriverEntity entity("TEST0000-0000-0001-0000-000000000001", 42.42, "Hello World");

	// Act
	EL_EDbOperationStatusCode resultCode = driver.AddOrUpdate(entity);

	// Assert
	if (!resultCode == EL_EDbOperationStatusCode.SUCCESS) return new EL_TestResult(false);

	array<ref EL_DbEntity> results = driver.FindAll(EL_Test_InMemoryDbDriverEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();

	if (results.Count() != 1) return new EL_TestResult(false);

	EL_Test_InMemoryDbDriverEntity resultEntity = EL_Test_InMemoryDbDriverEntity.Cast(results.Get(0));

	if (!resultEntity) return new EL_TestResult(false);

	return new EL_TestResult(
		resultEntity.GetId() == entity.GetId() &&
		resultEntity.m_fFloatValue == entity.m_fFloatValue &&
		resultEntity.m_sStringValue == entity.m_sStringValue);
}

//------------------------------------------------------------------------------------------------
[Test("EL_InMemoryDbDriverTests")]
TestResultBase EL_Test_InMemoryDbDriver_Remove_ExistingId_Removed()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	driver.Initalize("testing");

	EL_Test_InMemoryDbDriverEntity entity("TEST0000-0000-0001-0000-000000000002", 42.42, "Hello World");

	driver.AddOrUpdate(entity);

	// Act
	EL_EDbOperationStatusCode resultCode = driver.Remove(EL_Test_InMemoryDbDriverEntity, entity.GetId());

	// Assert
	if (!resultCode == EL_EDbOperationStatusCode.SUCCESS) return new EL_TestResult(false);

	array<ref EL_DbEntity> results = driver.FindAll(EL_Test_InMemoryDbDriverEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();

	return new EL_TestResult(results.Count() == 0);
}

//------------------------------------------------------------------------------------------------
[Test("EL_InMemoryDbDriverTests")]
TestResultBase EL_Test_InMemoryDbDriver_Remove_UnknownId_Error()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	driver.Initalize("testing");

	EL_Test_InMemoryDbDriverEntity entity("TEST0000-0000-0001-0000-000000000003", 42.42, "Hello World");

	// Act
	EL_EDbOperationStatusCode resultCode = driver.Remove(EL_Test_InMemoryDbDriverEntity, "I-DO-NOT-EXIST");

	// Assert
	return new EL_TestResult(resultCode == EL_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND);
}
