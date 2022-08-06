class EL_TriggerSaveAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if(persistenceManager) persistenceManager.AutoSave();
	}
}
