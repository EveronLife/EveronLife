class DummyChild : EL_DbEntity
{
	float m_ChildFloat = 42.69;
	ref array<string> m_Strings = {"Hello", "World"};
}

class DummyParent : EL_DbEntity
{
	bool m_ParentBool = true;
	ref DummyChild m_Child = new DummyChild();
}

//-----------------------------------------------------------------------------
// EXAMPLES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//! Basic test result.
class TestBoolResult : TestResultBase
{
	bool Value;

	void TestBoolResult(bool val) { Value = val; }

	override bool Failure() { return !Value; }

	override string FailureText()
	{
		// junit kind of error report. (simple)
		return "<failure type=\"BoolResult\">Failed</failure>";
	}
}

//-----------------------------------------------------------------------------
/*class MyHarness : TestHarness
{
}*/

//-----------------------------------------------------------------------------
class MyFirstTestSuite : TestSuite
{
	[Step(EStage.Setup)]
	void Setup()
	{
		Print("MyFirstTestSuite::Setup");
	}

	[Step(EStage.TearDown)]
	void TearDown()
	{
		Print("MyFirstTestSuite::TearDown");
	}
}

//-----------------------------------------------------------------------------
[Test("MyFirstTestSuite")]
TestResultBase MyFooBarTest() 
{ 
	return TestBoolResult(5 > 3); 
}

[Test("MyFirstTestSuite")]
class MyAsyncTest : TestBase
{
	int counter;

	[Step(EStage.Main)]
	void Set()
	{
		counter = 10;
	}

	[Step(EStage.Main)]
	bool Pool() 
	{
		Print("AsyncTest::Pool::counter=" + counter);

		if(counter == 0)
		{
			Print("AsyncTest::Pool::Result");
			SetResult(new TestBoolResult(true));
			return true;
		}

		Print("AsyncTest::Pool::Progress");

		counter--;
		return false;
	}
}