class EL_UtilsTests : TestSuite
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
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_ZeroNoPadding_Zero()
{
	return new EL_TestResult(EL_Utils.IntToHex(0) == "0");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_ZeroPadding4_Zero4()
{
	return new EL_TestResult(EL_Utils.IntToHex(0, fixedLength: 4) == "0000");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_ZeroPadding10_Zero8()
{
	return new EL_TestResult(EL_Utils.IntToHex(0, fixedLength: 10) == "00000000");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_1000_3e8()
{
	return new EL_TestResult(EL_Utils.IntToHex(1000) == "3e8");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_1000Upper_3E8()
{
	return new EL_TestResult(EL_Utils.IntToHex(1000, true) == "3E8");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_13342UpperPadded_00341E()
{
	return new EL_TestResult(EL_Utils.IntToHex(13342, true, 6) == "00341E");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_IntMaxUpper_7FFFFFFF()
{
	return new EL_TestResult(EL_Utils.IntToHex(int.MAX, true) == "7FFFFFFF");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_MiddleZeros_b00b()
{
	return new EL_TestResult(EL_Utils.IntToHex(45067) == "b00b");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_MiddleZerosPadded_00b00b()
{
	return new EL_TestResult(EL_Utils.IntToHex(45067, fixedLength: 7) == "000b00b");
}

//------------------------------------------------------------------------------------------------
[Test("EL_UtilsTests")]
TestResultBase EL_Test_Utils_IntToHex_FixedExeeded_AllPresent()
{
	return new EL_TestResult(EL_Utils.IntToHex(4095, fixedLength: 2) == "fff");
}
