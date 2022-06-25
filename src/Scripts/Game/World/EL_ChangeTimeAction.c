class EL_ChangeTimeAction : ScriptedUserAction
{
	[Attribute(defvalue:"12", UIWidgets.Slider, desc: "Target Time", "0 23 1")]
	private int m_fTargetTime;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		TimeAndWeatherManagerEntity weatherManager = GetGame().GetTimeAndWeatherManager();
		if (!weatherManager) 
			return;
		
		weatherManager.SetTimeOfTheDay(m_fTargetTime, true);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		return true;
 	}

}
