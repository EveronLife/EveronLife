class EL_ChangeTimeAction : ScriptedUserAction
{
	[Attribute(defvalue:"12", UIWidgets.Slider, desc: "Target Time", "0 23 1")]
	private int m_fTargetTime;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RplComponent replication = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		if (replication && !replication.IsOwner()) return;

		ChimeraWorld world = pOwnerEntity.GetWorld();
		TimeAndWeatherManagerEntity weatherManager = world.GetTimeAndWeatherManager();
		if (weatherManager)
			weatherManager.SetTimeOfTheDay(m_fTargetTime, true);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return true;
	}
}
