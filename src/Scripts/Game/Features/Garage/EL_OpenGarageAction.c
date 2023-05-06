class EL_OpenGarageAction : ScriptedUserAction
{
	EL_GarageManagerComponent m_GarageManager;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_GarageManager.OpenGarage(pUserEntity);
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_GarageManager = EL_GarageManagerComponent.Cast(pOwnerEntity.FindComponent(EL_GarageManagerComponent));
	}
}
