modded class SCR_BaseGameMode
{
	void ~SCR_BaseGameMode()
	{
		// TODO: Find better even for "session" teardown that works on dedicated servers, workbench and player hosted missions.
		EL_DbContextFactory.ResetCache();
		EL_DbEntityRepositoryFactory.ResetCache();
	}
}
