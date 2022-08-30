class EL_DbFindConditionBuilderTests : TestSuite
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

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_And_EmptyArgs_EmptyCondition()
{
	// Act
	EL_DbFindAnd condition = EL_DbFind.And({});

	// Assert
	return new EL_TestResult(condition.m_Conditions.Count() == 0);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_And_MultipleConditions_MultipleWrapped()
{
	// Act
	EL_DbFindAnd condition = EL_DbFind.And({new EL_DbFindCondition(), new EL_DbFindCondition()});

	// Assert
	return new EL_TestResult(condition.m_Conditions.Count() == 2);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Or_EmptyArgs_EmptyCondition()
{
	// Act
	EL_DbFindOr condition = EL_DbFind.Or({});

	// Assert
	return new EL_TestResult(condition.m_Conditions.Count() == 0);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Or_MultipleConditions_MultipleWrapped()
{
	// Act
	EL_DbFindOr condition = EL_DbFind.Or({new EL_DbFindCondition(), new EL_DbFindCondition()});

	// Assert
	return new EL_TestResult(condition.m_Conditions.Count() == 2);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_SingleField_ValidBuilder()
{
	// Act
	EL_DbFindFieldCollectionHandlingBuilder builder = EL_DbFind.Field("fieldName");

	// Assert
	return new EL_TestResult(builder.m_sFieldPath == "fieldName" && builder.m_bInverted == false);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_SingleFieldMultiValueInverted_ValidBuilder()
{
	// Act
	EL_DbFindFieldAllValueConditonBuilder builder = EL_DbFind.Field("fieldName.subField").Not();

	// Assert
	return new EL_TestResult(builder.m_sFieldPath == "fieldName.subField" && builder.m_bInverted == true);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_MultiField_FieldsChained()
{
	// Act
	EL_DbFindFieldCollectionHandlingBuilder builder = EL_DbFind.Field("fieldName").Field("subField");

	// Assert
	return new EL_TestResult(builder.m_sFieldPath == "fieldName.subField");
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_Length_ModifierPresent()
{
	// Act
	EL_DbFindFieldNumericValueConditonBuilder builder = EL_DbFind.Field("stringField").Length();

	// Assert
	return new EL_TestResult(builder.m_sFieldPath.EndsWith("stringField:count"));
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_Count_ModifierPresent()
{
	// Act
	EL_DbFindFieldNumericValueConditonBuilder builder = EL_DbFind.Field("collectionName").Count();

	// Assert
	return new EL_TestResult(builder.m_sFieldPath.EndsWith("collectionName:count"));
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_Any_ModifierPresent()
{
	// Act
	EL_DbFindFieldMainConditionBuilder builder = EL_DbFind.Field("collectionName").Any();

	// Assert
	return new EL_TestResult(builder.m_sFieldPath.EndsWith(":any"));
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_All_ModifierPresent()
{
	// Act
	EL_DbFindFieldMainConditionBuilder builder = EL_DbFind.Field("collectionName").All();

	// Assert
	return new EL_TestResult(builder.m_sFieldPath.EndsWith(":all"));
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_Keys_ModifierPresent()
{
	// Act
	EL_DbFindFieldCollectionHandlingBuilder builder = EL_DbFind.Field("collectionName").Keys();

	// Assert
	return new EL_TestResult(builder.m_sFieldPath.EndsWith(":keys"));
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_Values_ModifierPresent()
{
	// Act
	EL_DbFindFieldCollectionHandlingBuilder builder = EL_DbFind.Field("collectionName").Values();

	// Assert
	return new EL_TestResult(builder.m_sFieldPath.EndsWith(":values"));
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_ValuesAny_ModifiersPresent()
{
	// Act
	EL_DbFindFieldMainConditionBuilder builder = EL_DbFind.Field("collectionName").Values().Any();

	// Assert
	return new EL_TestResult(builder.m_sFieldPath.EndsWith(":values:any"));
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_At_IndexFieldSet()
{
	// Act
	EL_DbFindFieldMainConditionBuilder builder = EL_DbFind.Field("collectionName").At(3);

	// Assert
	return new EL_TestResult(builder.m_sFieldPath == "collectionName.3");
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_FirstOf_ModiferAndTypefilterPresent()
{
	// Act
	EL_DbFindFieldMainConditionBuilder builder = EL_DbFind.Field("collectionName").FirstOf(Class);

	// Assert
	return new EL_TestResult(builder.m_sFieldPath == "collectionName:any.Class");
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
TestResultBase EL_Test_DbFindConditionBuilder_Field_AllOf_ModiferAndTypefilterPresent()
{
	// Act
	EL_DbFindFieldMainConditionBuilder builder = EL_DbFind.Field("collectionName").AllOf(Class);

	// Assert
	return new EL_TestResult(builder.m_sFieldPath == "collectionName:all.Class");
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbFindConditionBuilderTests")]
class EL_Test_DbFindConditionBuilder_Field_ComplexBuild_DebugStringEqual : TestBase
{
	[Step(EStage.Main)]
	void DoTest()
	{
		// Arrange
		EL_DbFindCondition condition = EL_DbFind.Or({
			EL_DbFind.Field("A").Not().Null(),
			EL_DbFind.Field("B").Empty(),
			EL_DbFind.And({
				EL_DbFind.Field("CString").Contains("SubString"),
				EL_DbFind.Field("DFloatArray").Equals(EL_DbValues<bool>.From({true, false, true, true})),
				EL_DbFind.And({
					EL_DbFind.Field("E.m_Numbers").Contains(100),
					EL_DbFind.Field("F.m_ComplexWrapperSet").FirstOf(Class).Field("someNumber").Not().EqualsAnyOf(EL_DbValues<int>.From({1, 2}))
				}),
				EL_DbFind.Or({
					EL_DbFind.Field("G").EqualsAnyOf(EL_DbValues<int>.From({12, 13}))
				})
			})
		});

		// Act
		string debugString = condition.GetDebugString();
		debugString.Replace("\t", "");
		debugString.Replace("    ", "");

		// Assert
		string compareString = "Or(\
	    CheckNull(fieldPath:'A', shouldBeNull:false),\
	    CheckEmpty(fieldPath:'B', shouldBeEmpty:true),\
	    And(\
	        Compare(fieldPath:'CString', operator:CONTAINS, values:{SubString}),\
	        Compare(fieldPath:'DFloatArray', operator:EQUAL, values:{true,false,true,true}),\
	        And(\
	            Compare(fieldPath:'E.m_Numbers:any', operator:CONTAINS, values:{100}),\
	            Compare(fieldPath:'F.m_ComplexWrapperSet:any.Class.someNumber', operator:NOT_EQUAL, values:{1,2})\
	        ),\
	        Or(\
	            Compare(fieldPath:'G', operator:EQUAL, values:{12,13})\
	        )\
	    )\n)";
		compareString.Replace("\r", "");
		compareString.Replace("\t", "");
		compareString.Replace("    ", "");

		SetResult(new EL_TestResult(debugString == compareString));
	}
}
