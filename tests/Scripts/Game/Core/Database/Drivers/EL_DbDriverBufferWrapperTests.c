/*
class EL_DbDriverBufferWrapperTests : TestSuite
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
};

class EL_Test_DbDriverBufferWrapperEntity : EL_DbEntity
{
	int m_iValue;

	//------------------------------------------------------------------------------------------------
	static EL_Test_DbDriverBufferWrapperEntity Create(string id, int value)
	{
		EL_Test_DbDriverBufferWrapperEntity instance();
		instance.SetId(id);
		instance.m_iValue = value;
		return instance;
	}
};

//------------------------------------------------------------------------------------------------
[Test("EL_DbDriverBufferWrapperTests")]
TestResultBase EL_Test_DbDriverBufferWrapper_AddOrUpdateFindById_NotFlushed_Returned()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	EL_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initalize(connectInfo);
	EL_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);

	// Act
	EL_EDbOperationStatusCode resultCode = bufferedDriver.AddOrUpdate(entity);

	// Assert
	if (resultCode != EL_EDbOperationStatusCode.SUCCESS)
		return new EL_TestResult(false);

	array<ref EL_DbEntity> results = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();
	if (results.Count() != 1)
		return new EL_TestResult(false);

	EL_Test_DbDriverBufferWrapperEntity resultEntity = EL_Test_DbDriverBufferWrapperEntity.Cast(results.Get(0));
	return new EL_TestResult(
		resultEntity.GetId() == entity.GetId() &&
		resultEntity.m_iValue == entity.m_iValue);
};

//------------------------------------------------------------------------------------------------
[Test("EL_DbDriverBufferWrapperTests")]
TestResultBase EL_Test_DbDriverBufferWrapper_AddOrUpdateInPlaceTwice_Flushed_Returned()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	EL_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initalize(connectInfo);
	EL_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);
	auto updatedEntity = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 1337);

	// Act
	EL_EDbOperationStatusCode resultCode1 = bufferedDriver.AddOrUpdate(entity);
	EL_EDbOperationStatusCode resultCode2 = bufferedDriver.AddOrUpdate(updatedEntity);
	bufferedDriver.Flush(forceBlocking: true);

	// Assert
	if (resultCode1 != EL_EDbOperationStatusCode.SUCCESS||
		resultCode2 != EL_EDbOperationStatusCode.SUCCESS)
	{
		return new EL_TestResult(false);
	}

	array<ref EL_DbEntity> results = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, EL_DbFind.Id().Equals(updatedEntity.GetId())).GetEntities();
	if (results.Count() != 1)
		return new EL_TestResult(false);

	EL_Test_DbDriverBufferWrapperEntity resultEntity = EL_Test_DbDriverBufferWrapperEntity.Cast(results.Get(0));
	return new EL_TestResult(
		resultEntity.GetId() == updatedEntity.GetId() &&
		resultEntity.m_iValue == updatedEntity.m_iValue);
};

//------------------------------------------------------------------------------------------------
[Test("EL_DbDriverBufferWrapperTests")]
TestResultBase EL_Test_DbDriverBufferWrapper_AddOrUpdateFindAllLimited_NotFlushed_ReturnedOnlyInLimit()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	EL_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initalize(connectInfo);
	EL_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity1 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);
	auto entity2 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000002", 43);
	auto entity3 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000003", 44);

	// Act
	EL_EDbOperationStatusCode resultCode1 = bufferedDriver.AddOrUpdate(entity1);
	EL_EDbOperationStatusCode resultCode2 = bufferedDriver.AddOrUpdate(entity2);
	EL_EDbOperationStatusCode resultCode3 = bufferedDriver.AddOrUpdate(entity3);

	// Assert
	if (resultCode1 != EL_EDbOperationStatusCode.SUCCESS||
		resultCode2 != EL_EDbOperationStatusCode.SUCCESS||
		resultCode3 != EL_EDbOperationStatusCode.SUCCESS)
	{
		return new EL_TestResult(false);
	}

	array<ref EL_DbEntity> results = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, limit: 2).GetEntities();
	if (results.Count() != 2)
		return new EL_TestResult(false);

	EL_Test_DbDriverBufferWrapperEntity resultEntity1 = EL_Test_DbDriverBufferWrapperEntity.Cast(results.Get(0));
	EL_Test_DbDriverBufferWrapperEntity resultEntity2 = EL_Test_DbDriverBufferWrapperEntity.Cast(results.Get(1));
	return new EL_TestResult(
		resultEntity1.GetId() == entity1.GetId() &&
		resultEntity2.GetId() == entity2.GetId());
};

//------------------------------------------------------------------------------------------------
[Test("EL_DbDriverBufferWrapperTests")]
TestResultBase EL_Test_DbDriverBufferWrapper_AddOrUpdateFindMultipleByIdDesc_NotFlushed_Returned()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	EL_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initalize(connectInfo);
	EL_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity1 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);
	auto entity2 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000002", 43);
	auto entity3 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000003", 44);

	// Act
	EL_EDbOperationStatusCode resultCode1 = bufferedDriver.AddOrUpdate(entity1);
	EL_EDbOperationStatusCode resultCode2 = bufferedDriver.AddOrUpdate(entity2);
	EL_EDbOperationStatusCode resultCode3 = bufferedDriver.AddOrUpdate(entity3);

	// Assert
	if (resultCode1 != EL_EDbOperationStatusCode.SUCCESS||
		resultCode2 != EL_EDbOperationStatusCode.SUCCESS||
		resultCode3 != EL_EDbOperationStatusCode.SUCCESS)
	{
		return new EL_TestResult(false);
	}

	array<ref EL_DbEntity> results = bufferedDriver.FindAll(
		EL_Test_DbDriverBufferWrapperEntity,
		EL_DbFind.Id().EqualsAnyOf(EL_DbValues<string>.From({"TEST0000-0000-0001-0000-000000000002", "TEST0000-0000-0001-0000-000000000003"})),
		orderBy: {{"m_iValue", EL_DbEntitySortDirection.DESCENDING}}
	).GetEntities();

	if (results.Count() != 2)
		return new EL_TestResult(false);

	EL_Test_DbDriverBufferWrapperEntity resultEntity1 = EL_Test_DbDriverBufferWrapperEntity.Cast(results.Get(0));
	EL_Test_DbDriverBufferWrapperEntity resultEntity2 = EL_Test_DbDriverBufferWrapperEntity.Cast(results.Get(1));
	return new EL_TestResult(
		resultEntity1.GetId() == entity3.GetId() &&
		resultEntity2.GetId() == entity2.GetId());
};

//------------------------------------------------------------------------------------------------
[Test("EL_DbDriverBufferWrapperTests")]
TestResultBase EL_Test_DbDriverBufferWrapper_AddRemove_NotFlushed_NotRetured()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	EL_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initalize(connectInfo);
	EL_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);

	// Act
	EL_EDbOperationStatusCode resultCode1 = bufferedDriver.AddOrUpdate(entity);
	EL_EDbOperationStatusCode resultCode2 = bufferedDriver.Remove(EL_Test_DbDriverBufferWrapperEntity, entity.GetId());

	// Assert
	if (resultCode1 != EL_EDbOperationStatusCode.SUCCESS ||
		resultCode2 != EL_EDbOperationStatusCode.SUCCESS)
	{
		return new EL_TestResult(false);
	}

	array<ref EL_DbEntity> results = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();
	return new EL_TestResult(results.Count() == 0);
};

//------------------------------------------------------------------------------------------------
[Test("EL_DbDriverBufferWrapperTests")]
TestResultBase EL_Test_DbDriverBufferWrapper_AddRemove_Flushed_NotRetured()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	EL_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initalize(connectInfo);
	EL_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);

	// Act
	EL_EDbOperationStatusCode resultCode1 = bufferedDriver.AddOrUpdate(entity);
	EL_EDbOperationStatusCode resultCode2 = bufferedDriver.Remove(EL_Test_DbDriverBufferWrapperEntity, entity.GetId());
	bufferedDriver.Flush(forceBlocking: true);

	// Assert
	if (resultCode1 != EL_EDbOperationStatusCode.SUCCESS ||
		resultCode2 != EL_EDbOperationStatusCode.SUCCESS)
	{
		return new EL_TestResult(false);
	}

	array<ref EL_DbEntity> results = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();
	return new EL_TestResult(results.Count() == 0);
};

//------------------------------------------------------------------------------------------------
[Test("EL_DbDriverBufferWrapperTests")]
TestResultBase EL_Test_DbDriverBufferWrapper_AddFlushRemove_NotFlushed_NotRetured()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	EL_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initalize(connectInfo);
	EL_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);

	// Act
	EL_EDbOperationStatusCode resultCode1 = bufferedDriver.AddOrUpdate(entity);
	bufferedDriver.Flush(forceBlocking: true);
	EL_EDbOperationStatusCode resultCode2 = bufferedDriver.Remove(EL_Test_DbDriverBufferWrapperEntity, entity.GetId());

	// Assert
	if (resultCode1 != EL_EDbOperationStatusCode.SUCCESS ||
		resultCode2 != EL_EDbOperationStatusCode.SUCCESS)
	{
		return new EL_TestResult(false);
	}

	array<ref EL_DbEntity> bufferedResults = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();
	array<ref EL_DbEntity> dbResults = driver.FindAll(EL_Test_DbDriverBufferWrapperEntity, EL_DbFind.Id().Equals(entity.GetId())).GetEntities();
	return new EL_TestResult(bufferedResults.Count() == 0 && dbResults.Count() == 1);
};

//------------------------------------------------------------------------------------------------
[Test("EL_DbDriverBufferWrapperTests")]
TestResultBase EL_Test_DbDriverBufferWrapper_FindAllPaginatedOrdered_HalfFlushed_CorrectOrder()
{
	// Arrange
	EL_InMemoryDbDriver driver();
	EL_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initalize(connectInfo);
	EL_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity1 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 11);
	auto entity2 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000002", 22);
	auto entity3 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000003", 33); // Deleted, Not flushed
	auto entity4 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000004", 44); // Deleted, Not flushed
	auto entity5 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000005", 55);
	auto entity6 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000006", 66);
	auto entity7 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000007", 77);
	auto entity8 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000008", 88); //Not flushed
	auto entity9 = EL_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000009", 99); //Not flushed

	// Act
	bufferedDriver.AddOrUpdate(entity1);
	bufferedDriver.AddOrUpdate(entity2);
	//bufferedDriver.AddOrUpdate(entity3);
	//bufferedDriver.AddOrUpdate(entity4);
	bufferedDriver.AddOrUpdate(entity5);
	bufferedDriver.AddOrUpdate(entity6);
	bufferedDriver.AddOrUpdate(entity7);
	bufferedDriver.Flush(forceBlocking: true);
	//bufferedDriver.Remove(EL_Test_DbDriverBufferWrapperEntity, entity3.GetId());
	//bufferedDriver.Remove(EL_Test_DbDriverBufferWrapperEntity, entity4.GetId());
	bufferedDriver.AddOrUpdate(entity3);
	bufferedDriver.AddOrUpdate(entity4);
	bufferedDriver.AddOrUpdate(entity8);
	bufferedDriver.AddOrUpdate(entity9);

	// Assert

	EL_DbFindCondition condition = null; //EL_DbFind.Id().Equals(entity.GetId())
	array<ref TStringArray> orderBy = {{"m_iValue", EL_DbEntitySortDirection.ASCENDING}}; //null;

	array<ref EL_DbEntity> page1 = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 0, limit: 3).GetEntities();
	array<ref EL_DbEntity> page2 = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 3, limit: 3).GetEntities();
	array<ref EL_DbEntity> page3 = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 6, limit: 3).GetEntities();

	//array<ref EL_DbEntity> page1 = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 0, limit: 2).GetEntities();
	//array<ref EL_DbEntity> page2 = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 2, limit: 2).GetEntities();
	//array<ref EL_DbEntity> page3 = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 4, limit: 2).GetEntities();
	//array<ref EL_DbEntity> page4 = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 6, limit: 2).GetEntities();
	//array<ref EL_DbEntity> page5 = bufferedDriver.FindAll(EL_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 8, limit: 2).GetEntities();

	return new EL_TestResult(true);
};
*/
