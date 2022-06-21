class EL_ChangeTimeAction : ScriptedUserAction
{
	[Attribute(defvalue:"12", UIWidgets.Slider, desc: "Target Time", "0 23 1")]
	private int m_fTargetTime;

	
	//------------------------------------------------------------------------------------------------
	// User has performed the action
	// play a pickup sound and then add the correct amount to the users inventory
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		TimeAndWeatherManagerEntity weatherManager = GetGame().GetTimeAndWeatherManager();
		if (!weatherManager) 
			return;
		
		weatherManager.SetTimeOfTheDay(m_fTargetTime, true);
	}
	
	//------------------------------------------------------------------------------------------------
	// Checks if a required item has been set in the Editor
	// If so, check if its in the users inventory/hands depending on settings set
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
 	}

}
