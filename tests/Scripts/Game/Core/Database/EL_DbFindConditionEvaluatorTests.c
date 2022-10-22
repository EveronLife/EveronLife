class EL_DbFindConditionEvaluatorTests : TestSuite
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

class EL_Test_FindConditionEvaluatorContainerItem : EL_DbEntity
{
	int m_Int = 1337;
	float m_Float = 42.42;
	bool m_Bool = true;
	string m_String = "Hello World";
	vector m_Vector = Vector(1, 3, 7);

	EL_Test_FindConditionEvaluatorContainerItem m_NullField;

	ref array<int> m_IntArray = {42, 1337};
	ref array<float> m_FloatArray = {42.42, 1337.1337};
	ref array<bool> m_BoolArray = {true, false, true, false};
	ref array<string> m_StringArray = {"Hello", "World"};
	ref array<vector> m_VectorArray = {Vector(1, 3, 7), Vector(1, 0, 1)};

	ref set<float> m_FloatSet;

	//------------------------------------------------------------------------------------------------
	void EL_Test_FindConditionEvaluatorContainerItem()
	{
		m_FloatSet = new set<float>();
		m_FloatSet.Insert(42.42);
		m_FloatSet.Insert( 1337.1337);
	}
}

class EL_Test_FindConditionEvaluatorContainer : EL_DbEntity
{
	ref EL_Test_FindConditionEvaluatorContainerItem m_SingleItem;
	ref array<ref EL_Test_FindConditionEvaluatorContainerItem> m_MultiItemArray;
	ref set<ref EL_Test_FindConditionEvaluatorContainerItem> m_MultiItemSet;
	ref map<string, ref EL_Test_FindConditionEvaluatorContainerItem> m_MultiItemMap;

	ref map<string, int> m_IntMap;

	//------------------------------------------------------------------------------------------------
	void EL_Test_FindConditionEvaluatorContainer()
	{
		m_SingleItem = new EL_Test_FindConditionEvaluatorContainerItem();

		m_MultiItemArray = new array<ref EL_Test_FindConditionEvaluatorContainerItem>();
		m_MultiItemArray.Insert(new EL_Test_FindConditionEvaluatorContainerItem());
		m_MultiItemArray.Insert(new EL_Test_FindConditionEvaluatorContainerItem());

		m_MultiItemSet = new set<ref EL_Test_FindConditionEvaluatorContainerItem>();
		m_MultiItemSet.Insert(new EL_Test_FindConditionEvaluatorContainerItem());
		m_MultiItemSet.Insert(new EL_Test_FindConditionEvaluatorContainerItem());

		m_MultiItemMap = new map<string, ref EL_Test_FindConditionEvaluatorContainerItem>();
		m_MultiItemMap.Set("key1", new EL_Test_FindConditionEvaluatorContainerItem());
		m_MultiItemMap.Set("key2", new EL_Test_FindConditionEvaluatorContainerItem());

		m_IntMap = new map<string, int>();
		m_IntMap.Set("1337", 1337);
		m_IntMap.Set("42", 42);
	}
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_CorrectInt_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Int").Equals(1337);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_IncorrectInt_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Int").Equals(1000);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_CorrectFloat_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Float").Equals(42.42);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_IncorrectFloat_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Int").Equals(10.13);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_CorrectBool_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Bool").Equals(true);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_IncorrectBool_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Bool").Equals(false);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_CorrectString_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_String").Equals("Hello World");

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_IncorrectString_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_String").Equals("Noot noot.");

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_CorrectVector_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Vector").Equals(Vector(1, 3, 7));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_IncorrectVector_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Vector").Equals(Vector(6, 6, 6));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Contains_StringField_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_String").Contains("Hello");

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_And_OneTrue_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.And({
		EL_DbFind.Field("m_Bool").Equals(true),
		EL_DbFind.Field("m_Int").Equals(100),
	});

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_And_BothTrue_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.And({
		EL_DbFind.Field("m_Bool").Equals(true),
		EL_DbFind.Field("m_Int").Equals(1337),
	});

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Or_NoneTrue_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Or({
		EL_DbFind.Field("m_Bool").Equals(false),
		EL_DbFind.Field("m_Int").Equals(100),
	});

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Or_OneTrue_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Or({
		EL_DbFind.Field("m_Bool").Equals(false),
		EL_DbFind.Field("m_Int").Equals(1337),
	});

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_EqualsAnyOf_OneTrue_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Float").EqualsAnyOf(EL_DbValues<float>.From({3.14, 2.22, 42.42}));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_EqualsAnyOf_NoneTrue_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Float").EqualsAnyOf(EL_DbValues<float>.From({3.14, 2.22, 3.33}));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_LessThan_FloatLess_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Float").LessThan(100.5);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_LessThanOrEqual_IntGreater_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Int").LessThanOrEqual(100);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_GreaterThanOrEqual_VectorLess_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Vector").GreaterThanOrEqual(Vector(1, 1, 1));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Between_InRange_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Int").Between(1300, 1400);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Not_Matching_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_Bool").Not().Equals(true);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Null_NullField_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_NullField").Null();

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Empty_FilledCollection_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_BoolArray").Empty();

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Count_AnyOfIncludingTwo_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_IntArray").Count().EqualsAnyOf(EL_DbValues<int>.From({1, 2, 3}));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Any_MatchingValue_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_IntArray").Any().GreaterThanOrEqual(1300);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_All_OneNotMatching_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_IntArray").All().LessThanOrEqual(100);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Keys_OneMatching_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainer entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_MultiItemMap").Keys().Any().Equals("key2");

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Values_OneMatching_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainer entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_IntMap").Values().Any().EqualsAnyOf(EL_DbValues<int>.From({10, 1300, 42}));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_At_IdxZeroNestedVectorContained_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainer entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_MultiItemArray").At(0).Field("m_VectorArray").Contains(Vector(1, 0, 1));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_ContainsAnyOf_PartialIntersect_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_FloatSet").ContainsAnyOf(EL_DbValues<float>.From({666.666, 1337.1337}));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_ContainsAllOf_PartialIntersect_False()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_FloatSet").ContainsAllOf(EL_DbValues<float>.From({666.666, 1337.1337}));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(!matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Equals_ArrayMatches_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_FloatArray").Equals(EL_DbValues<float>.From({42.42, 1337.1337}));

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_Contains_StringArrayItemMatches_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorContainerItem entity();

	EL_DbFindCondition conditon = EL_DbFind.Field("m_StringArray").Contains("World");

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EL_TestResult(matches);
}

class EL_Test_FindConditionEvaluatorPolymorphBase : EL_DbEntity
{
	ref array<ref EL_Test_FindConditionEvaluatorPolymorphBase> m_NestedArrayItems = {};
}

class EL_Test_FindConditionEvaluatorPolymorphDerivedA : EL_Test_FindConditionEvaluatorPolymorphBase
{
	int m_intFieldA;

	//------------------------------------------------------------------------------------------------
	void EL_Test_FindConditionEvaluatorPolymorphDerivedA(int value = 0)
	{
		m_intFieldA = value;
	}
}

class EL_Test_FindConditionEvaluatorPolymorphDerivedB : EL_Test_FindConditionEvaluatorPolymorphBase
{
	float m_floatFieldB;

	//------------------------------------------------------------------------------------------------
	void EL_Test_FindConditionEvaluatorPolymorphDerivedB(float value = 0.0)
	{
		m_floatFieldB = value;
	}
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
TestResultBase EL_Test_DbFindConditionEvaluator_FirstOf_Matches_True()
{
	// Arrange
	EL_Test_FindConditionEvaluatorPolymorphBase container();
	container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedB());
	container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedA(10));
	container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedA(1337));
	container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedB());
	container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedA(20));
	container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedB());

	EL_DbFindCondition conditon = EL_DbFind
		.Field("m_NestedArrayItems")
		.FirstOf(EL_Test_FindConditionEvaluatorPolymorphDerivedA)
		.Field("m_intFieldA")
		.Equals(10);

	// Act
	bool matches = EL_DbFindConditionEvaluator.Evaluate(container, conditon);

	// Assert
	return new EL_TestResult(matches);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionEvaluatorTests")]
class EL_Test_DbFindConditionEvaluator_AllOf_ConstMatches_True : TestBase
{
	static const ref EL_DbFindCondition s_Condition = EL_DbFind
		.Field("m_NestedArrayItems")
		.AllOf(EL_Test_FindConditionEvaluatorPolymorphDerivedB)
		.Field("m_floatFieldB")
		.GreaterThanOrEqual(42.0);

	[Step(EStage.Main)]
	void DoTest()
	{
		// Arrange
		EL_Test_FindConditionEvaluatorPolymorphBase container();
		container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedB(42.1));
		container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedB(42.2));
		container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EL_Test_FindConditionEvaluatorPolymorphDerivedB(42.42));

		// Act
		bool matches = EL_DbFindConditionEvaluator.Evaluate(container, s_Condition);

		// Assert
		SetResult(new EL_TestResult(matches));
	}
}
