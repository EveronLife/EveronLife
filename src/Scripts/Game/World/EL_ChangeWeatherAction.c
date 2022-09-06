class EL_ChangeWeatherAction : ScriptedUserAction
{
	[Attribute(defvalue:"", UIWidgets.Slider, desc: "Weather ID", "0 3 1")]
	private int m_fTargetWeatherID;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RplComponent replication = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		if (replication && !replication.IsOwner()) return;
		
		TimeAndWeatherManagerEntity weatherManager = GetGame().GetTimeAndWeatherManager();
		if (!weatherManager) return;
				
		array<ref WeatherState> weatherStates = new array<ref WeatherState>;
		weatherManager.GetWeatherStatesList(weatherStates);
		if (m_fTargetWeatherID >= weatherStates.Count()) return;
		
		weatherManager.ForceWeatherTo(true, weatherStates[m_fTargetWeatherID].GetStateName());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
 	}
}
