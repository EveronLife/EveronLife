class EL_SaveLoadoutAction : ScriptedUserAction
{
	protected EL_LoadoutManagerComponent m_LoadoutManagerComponent;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_LoadoutManagerComponent.SaveLoadout(GameEntity.Cast(pUserEntity));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Init
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_LoadoutManagerComponent = EL_LoadoutManagerComponent.Cast(pOwnerEntity.FindComponent(EL_LoadoutManagerComponent));
	}
}
