class EL_OpenVehicleShopAction : ScriptedUserAction
{
	EL_VehicleShopManagerComponent m_VehicleShopManager;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_VehicleShopManager.OpenVehicleShop(pUserEntity);
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_VehicleShopManager = EL_VehicleShopManagerComponent.Cast(pOwnerEntity.FindComponent(EL_VehicleShopManagerComponent));
	}
}
