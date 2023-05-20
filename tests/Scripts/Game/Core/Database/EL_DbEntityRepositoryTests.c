class EL_DbRepositoryTests : TestSuite
{
	static ref EL_DbContext m_pDbContext;
	
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
    void Setup()
    {
		m_pDbContext = EL_DbContext.Create("inmemory://DbRepositoryTests");
    }

	//------------------------------------------------------------------------------------------------
    [Step(EStage.TearDown)]
    void TearDown()
    {
		m_pDbContext = null;
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
	EL_DbFindResultSingle<EL_Test_DbEntityRepositoryEntity> FindByIntValue(int value)
	{
		return FindFirst(EL_DbFind.Field("m_iIntValue").Equals(value));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbRepositoryTests")]
TestResultBase EL_Test_DbEntityRepository_AddOrUpdate_NewEntityFindByIntValue_Found()
{
	// Arrange
	EL_Test_DbEntityRepositoryEntityRepository repository = EL_DbRepositoryHelper<EL_Test_DbEntityRepositoryEntityRepository>.Get(EL_DbRepositoryTests.m_pDbContext);

	EL_Test_DbEntityRepositoryEntity entity("TEST0000-0000-0001-0000-000000000001", 1001)
	
	// Act
	repository.AddOrUpdate(entity);

	// Assert
	EL_TestResult result(repository.FindByIntValue(1001).GetEntity().GetId() == "TEST0000-0000-0001-0000-000000000001");
	
	// Cleanup
	repository.Remove(entity);
	
	return result;
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbRepositoryTests")]
TestResultBase EL_Test_DbEntityRepository_Remove_ByInstance_Removed()
{
	// Arrange
	EL_DbRepository<EL_Test_DbEntityRepositoryEntity> repository = EL_DbEntityHelper<EL_Test_DbEntityRepositoryEntity>.GetRepository(EL_DbRepositoryTests.m_pDbContext);

	EL_Test_DbEntityRepositoryEntity entity("TEST0000-0000-0001-0000-000000000002", 1002);
	repository.AddOrUpdate(entity);

	// Act
	EL_EDbOperationStatusCode statusCode = repository.Remove(entity);

	// Assert
	EL_TestResult result(
		statusCode == EL_EDbOperationStatusCode.SUCCESS &&
		!repository.Find("TEST0000-0000-0001-0000-000000000002").GetEntity());
	
	// Cleanup
	repository.Remove(entity);
	
	return result;
}
