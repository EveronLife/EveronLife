class EL_DbEntityUtilsTests : TestSuite
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

class EL_Test_DbEntityUtilsSaveStruct : EL_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;
}

class EL_Test_DbEntityUtilsOtherClassType
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	private void EL_Test_DbEntityUtilsOtherClassType(float floatValue, string stringValue);
}

//------------------------------------------------------------------------------------------------
[Test("EL_DbEntityUtilsTests")]
TestResultBase EL_Test_DbEntityUtils_StructAutoCopy_ValidInput_MatchingOutput()
{
	// Arrange
	EL_Test_DbEntityUtilsSaveStruct saveStruct();
	saveStruct.m_fFloatValue = 42.42;
	saveStruct.m_sStringValue = "Hello World";

	EL_Test_DbEntityUtilsOtherClassType otherClass = EL_Test_DbEntityUtilsOtherClassType.Cast(String("EL_Test_DbEntityUtilsOtherClassType").ToType().Spawn());

	// Act
	EL_DbEntityUtils.StructAutoCopy(saveStruct, otherClass);

	// Assert
	return new EL_TestResult(otherClass && (otherClass.m_fFloatValue == saveStruct.m_fFloatValue) && (otherClass.m_sStringValue == saveStruct.m_sStringValue));
}
