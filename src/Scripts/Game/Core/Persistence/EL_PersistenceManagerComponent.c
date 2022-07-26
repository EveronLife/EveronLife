class EL_PersistenceManagerComponentClass : SCR_BaseGameModeComponentClass
{
}

class EL_PersistenceManagerComponent : SCR_BaseGameModeComponent
{
	override void OnWorldPostProcess(World world)
	{
		super.OnWorldPostProcess(world);
		
		EL_PersistenceManager.GetInstance().OnWorldPostProcess(world);
	}
	
	override void OnGameEnd()
	{
		super.OnGameEnd();
		
		EL_PersistenceManager.GetInstance().OnGameEnd();
	}
	
	/*
	override void EOnPostFrame(IEntity owner, float timeSlice)
	{
		EL_PersistenceManager.GetInstance().OnPostFrame(timeSlice);
	}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		SetEventMask(owner, EntityEvent.POSTFRAME);
	}
	*/
}
