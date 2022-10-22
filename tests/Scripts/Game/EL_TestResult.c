class EL_TestResult : TestResultBase
{
	protected bool m_Value;

	//------------------------------------------------------------------------------------------------
	void EL_TestResult(bool value)
	{
		m_Value = value;
	}

	//------------------------------------------------------------------------------------------------
	override bool Failure()
	{
		return !m_Value;
	}

	//------------------------------------------------------------------------------------------------
	override string FailureText()
	{
		return "<failure type=\"EL_TestResult\">Failed</failure>";
	}
}
