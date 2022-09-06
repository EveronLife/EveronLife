class EL_DbEntitySorterTests : TestSuite
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

class EL_Test_DbEntitySortableEntity : EL_DbEntity
{
	int m_iIntValue;
	float m_fFloatValue;
	bool m_bBoolValue;
	string m_sStringValue;
	vector m_vVectorValue;

	//------------------------------------------------------------------------------------------------
	void EL_Test_DbEntitySortableEntity(int intVal = 0, float floatVal = 0.0, bool boolVal = false, string stringVal = "", vector vectorVal = "0 0 0")
	{
		m_iIntValue = intVal;
		m_fFloatValue = floatVal;
		m_bBoolValue = boolVal;
		m_sStringValue = stringVal;
		m_vVectorValue = vectorVal;
	}
}

class EL_Test_DbEntitySortableEntitySingleWrapper : EL_DbEntity
{
	int m_iSameIntValue;
	ref EL_Test_DbEntitySortableEntity m_pEntity;

	//------------------------------------------------------------------------------------------------
	void EL_Test_DbEntitySortableEntitySingleWrapper(int intVal = 0, float floatVal = 0.0, bool boolVal = false, string stringVal = "", vector vectorVal = "0 0 0")
	{
		m_iSameIntValue = 1337;
		m_pEntity = new EL_Test_DbEntitySortableEntity(intVal, floatVal, boolVal, stringVal, vectorVal);
	}
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayInt_AscSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(intVal: 5),
		new EL_Test_DbEntitySortableEntity(intVal: 50),
		new EL_Test_DbEntitySortableEntity(intVal: 1)
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_iIntValue", EL_DbEntitySortDirection.ASCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_iIntValue == 1 &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_iIntValue == 5 &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_iIntValue == 50);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayInt_DescSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(intVal: 5),
		new EL_Test_DbEntitySortableEntity(intVal: 50),
		new EL_Test_DbEntitySortableEntity(intVal: 1)
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_iIntValue", EL_DbEntitySortDirection.DESCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_iIntValue == 50 &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_iIntValue == 5 &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_iIntValue == 1);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayFloat_AscSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(floatVal: 5.2),
		new EL_Test_DbEntitySortableEntity(floatVal: 50.3),
		new EL_Test_DbEntitySortableEntity(floatVal: 1.1)
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_fFloatValue", EL_DbEntitySortDirection.ASCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_fFloatValue == 1.1 &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_fFloatValue == 5.2 &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_fFloatValue == 50.3);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayFloat_DescSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(floatVal: 5.2),
		new EL_Test_DbEntitySortableEntity(floatVal: 50.3),
		new EL_Test_DbEntitySortableEntity(floatVal: 1.1)
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_fFloatValue", EL_DbEntitySortDirection.DESCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_fFloatValue == 50.3 &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_fFloatValue == 5.2 &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_fFloatValue == 1.1);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_WrappedIntFloatDirectionInvariant_DescSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntitySingleWrapper(floatVal: 5.2),
		new EL_Test_DbEntitySortableEntitySingleWrapper(floatVal: 50.3),
		new EL_Test_DbEntitySortableEntitySingleWrapper(floatVal: 1.1)
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_iSameIntValue", "Asc"}, {"m_pEntity.m_fFloatValue", "deSC"}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntitySingleWrapper.Cast(sorted.Get(0)).m_pEntity.m_fFloatValue == 50.3 &&
		EL_Test_DbEntitySortableEntitySingleWrapper.Cast(sorted.Get(1)).m_pEntity.m_fFloatValue == 5.2 &&
		EL_Test_DbEntitySortableEntitySingleWrapper.Cast(sorted.Get(2)).m_pEntity.m_fFloatValue == 1.1);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayBool_AscSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(boolVal: false),
		new EL_Test_DbEntitySortableEntity(boolVal: true),
		new EL_Test_DbEntitySortableEntity(boolVal: false)
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_bBoolValue", EL_DbEntitySortDirection.ASCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_bBoolValue == false &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_bBoolValue == false &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_bBoolValue == true);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayBool_DescSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(boolVal: false),
		new EL_Test_DbEntitySortableEntity(boolVal: true),
		new EL_Test_DbEntitySortableEntity(boolVal: false)
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_bBoolValue", EL_DbEntitySortDirection.DESCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_bBoolValue == true &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_bBoolValue == false &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_bBoolValue == false);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayString_AscSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(stringVal: "Hello World"),
		new EL_Test_DbEntitySortableEntity(stringVal: "987 ZyaD"),
		new EL_Test_DbEntitySortableEntity(stringVal: "876 AmrA"),
		new EL_Test_DbEntitySortableEntity(stringVal: "Hello Arma")
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_sStringValue", EL_DbEntitySortDirection.ASCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_sStringValue == "876 AmrA" &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_sStringValue == "987 ZyaD" &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_sStringValue == "Hello Arma" &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(3)).m_sStringValue == "Hello World");
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayString_DescSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(stringVal: "Hello World"),
		new EL_Test_DbEntitySortableEntity(stringVal: "987 ZyaD"),
		new EL_Test_DbEntitySortableEntity(stringVal: "876 AmrA"),
		new EL_Test_DbEntitySortableEntity(stringVal: "Hello Arma")
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_sStringValue", EL_DbEntitySortDirection.DESCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_sStringValue == "Hello World" &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_sStringValue == "Hello Arma" &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_sStringValue == "987 ZyaD" &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(3)).m_sStringValue == "876 AmrA");
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayVector_AscSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(vectorVal: Vector(1, 9, 1)),
		new EL_Test_DbEntitySortableEntity(vectorVal: Vector(1, 2, 1)),
		new EL_Test_DbEntitySortableEntity(vectorVal: Vector(0, 9, 9)),
		new EL_Test_DbEntitySortableEntity(vectorVal: Vector(3, 0, 0))
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_vVectorValue", EL_DbEntitySortDirection.ASCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_vVectorValue == Vector(0, 9, 9) &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_vVectorValue == Vector(1, 2, 1) &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_vVectorValue == Vector(1, 9, 1) &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(3)).m_vVectorValue == Vector(3, 0, 0));
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntitySorterTests")]
TestResultBase EL_Test_DbEntitySorter_GetSorted_ArrayVector_DescSorted()
{
	// Arrange
	array<ref EL_DbEntity> entities = {
		new EL_Test_DbEntitySortableEntity(vectorVal: Vector(1, 9, 1)),
		new EL_Test_DbEntitySortableEntity(vectorVal: Vector(1, 2, 1)),
		new EL_Test_DbEntitySortableEntity(vectorVal: Vector(0, 9, 9)),
		new EL_Test_DbEntitySortableEntity(vectorVal: Vector(3, 0, 0))
	};

	// Act
	array<ref EL_DbEntity> sorted = EL_DbEntitySorter.GetSorted(entities, {{"m_vVectorValue", EL_DbEntitySortDirection.DESCENDING}});

	// Assert
	return new EL_TestResult(
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_vVectorValue == Vector(3, 0, 0) &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_vVectorValue == Vector(1, 9, 1) &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_vVectorValue == Vector(1, 2, 1) &&
		EL_Test_DbEntitySortableEntity.Cast(sorted.Get(3)).m_vVectorValue == Vector(0, 9, 9));
}
