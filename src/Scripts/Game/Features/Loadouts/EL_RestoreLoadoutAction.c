class EL_RestoreLoadoutAction : ScriptedUserAction
{
	protected EL_LoadoutManagerComponent m_LoadoutManagerComponent;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_LoadoutManagerComponent.RestoreLoadout(GameEntity.Cast(pUserEntity));
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		return m_LoadoutManagerComponent.HasLoadout(GameEntity.Cast(user));
 	}
	
	//------------------------------------------------------------------------------------------------
	//! Init
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_LoadoutManagerComponent = EL_LoadoutManagerComponent.Cast(pOwnerEntity.FindComponent(EL_LoadoutManagerComponent));
	}
}
