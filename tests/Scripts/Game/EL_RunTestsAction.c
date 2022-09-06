class EL_RunTestsAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Run tests and wait until finished
		TestHarness.Begin();
		while (!TestHarness.Run()){}
		TestHarness.End();

		// Get and process results
		string testResults = TestHarness.Report();

		int year, month, day, hour, minute, second;
		System.GetYearMonthDayUTC(year, month, day);
		System.GetHourMinuteSecondUTC(hour, minute, second);
		string resultFile = string.Format("TestResults/Run %1-%2-%3 %4_%5_%6.xml", year, month, day, hour, minute, second);

		string hintText;

		if (!testResults.Contains("<failure ") && !testResults.Contains("<error "))
		{
			hintText = "All test run <color rgba='0,255,0,200'>successfully</color>.";
		}
		else
		{
			hintText = string.Format("Some tests <color rgba='255,0,0,200'>failed</color>! For details check MyGames/ArmaReforger/profile/</br>%1", resultFile);
		}

		SCR_HintManagerComponent.GetInstance().ShowCustomHint(hintText, duration: 30.0, isTimerVisible: true);

		// Write results file
		FileIO.MakeDirectory("$profile:TestResults");

		FileHandle handle = FileIO.OpenFile(string.Format("$profile:%1", resultFile), FileMode.WRITE);
		if (!handle) return;

		handle.FPrint(testResults);
		handle.CloseFile();
	}
}
