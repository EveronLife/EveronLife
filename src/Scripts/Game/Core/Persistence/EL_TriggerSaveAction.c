class EL_TriggerSaveAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RplComponent replication = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		if (replication && !replication.IsOwner()) return;

		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if(persistenceManager) persistenceManager.AutoSave();
	}
}
