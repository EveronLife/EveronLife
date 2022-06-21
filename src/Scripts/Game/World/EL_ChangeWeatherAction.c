class EL_ChangeWeatherAction : ScriptedUserAction
{
	[Attribute(defvalue:"", UIWidgets.Slider, desc: "Weather ID", "0 3 1")]
	private int m_fTargetWeatherID;

	
	//------------------------------------------------------------------------------------------------
	// User has performed the action
	// play a pickup sound and then add the correct amount to the users inventory
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		TimeAndWeatherManagerEntity weatherManager = GetGame().GetTimeAndWeatherManager();
		if (!weatherManager) 
			return;
				
		array<ref WeatherState> weatherStates = new array<ref WeatherState>;
		weatherManager.GetWeatherStatesList(weatherStates);
		
		//Valid state
		if (weatherStates.IsEmpty() || m_fTargetWeatherID >= weatherStates.Count())
			return;
		
		WeatherState weatherToSet = weatherStates[m_fTargetWeatherID];

		weatherManager.ForceWeatherTo(true, weatherToSet.GetStateName());
	}
	
	//------------------------------------------------------------------------------------------------
	// Checks if a required item has been set in the Editor
	// If so, check if its in the users inventory/hands depending on settings set
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
 	}

}
