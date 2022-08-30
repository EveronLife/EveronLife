class EL_DbRepositoryTests : TestSuite
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

class EL_Test_DbEntityRepositoryEntity : EL_DbEntity
{
	int m_iIntValue;

	//------------------------------------------------------------------------------------------------
	void EL_Test_DbEntityRepositoryEntity(string id, int intValue)
	{
		SetId(id);
		m_iIntValue = intValue;
	}
}

[EL_DbRepositoryType(EL_Test_DbEntityRepositoryEntityRepository, EL_Test_DbEntityRepositoryEntity)]
class EL_Test_DbEntityRepositoryEntityRepository : EL_DbRepository<EL_Test_DbEntityRepositoryEntity>
{
	EL_DbFindResult<EL_Test_DbEntityRepositoryEntity> FindByIntValue(int value)
	{
		return FindFirst(EL_DbFind.Field("m_iIntValue").Equals(value));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbRepositoryTests")]
TestResultBase EL_Test_DbEntityRepository_AddOrUpdate_NewEntityFindByIntValue_Found()
{
	// Arrange
	EL_DbContext dbContext = EL_DbContextFactory.GetContext();
	EL_Test_DbEntityRepositoryEntityRepository repository = EL_DbRepositoryHelper<EL_Test_DbEntityRepositoryEntityRepository>.Get(dbContext);

	// Act
	repository.AddOrUpdate(new EL_Test_DbEntityRepositoryEntity("TEST0000-0000-0001-0000-000000000001", 1001));

	// Assert
	return new EL_TestResult(repository.FindByIntValue(1001).GetEntity().GetId() == "TEST0000-0000-0001-0000-000000000001");
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbRepositoryTests")]
TestResultBase EL_Test_DbEntityRepository_Remove_ByInstance_Removed()
{
	// Arrange
	EL_DbContext dbContext = EL_DbContextFactory.GetContext();
	EL_DbRepository<EL_Test_DbEntityRepositoryEntity> repository = EL_DbEntityHelper<EL_Test_DbEntityRepositoryEntity>.GetRepository(dbContext);

	EL_Test_DbEntityRepositoryEntity entity("TEST0000-0000-0001-0000-000000000002", 1002);

	repository.AddOrUpdate(entity);

	// Act
	EL_EDbOperationStatusCode statusCode = repository.Remove(entity);

	// Assert
	return new EL_TestResult(
		statusCode == EL_EDbOperationStatusCode.SUCCESS &&
		repository.Find("TEST0000-0000-0001-0000-000000000002").GetStatusCode() == EL_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND);
}
