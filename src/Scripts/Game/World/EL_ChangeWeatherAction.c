class EL_ChangeWeatherAction : ScriptedUserAction
{
	[Attribute(defvalue:"", UIWidgets.Slider, desc: "Weather ID", "0 3 1")]
	private int m_fTargetWeatherID;

	//------------------------------------------------------------------------------------------------
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
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
 	}

}
