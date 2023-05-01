modded class SCR_RespawnComponent
{
	protected ref array<RplId> EL_m_aQuickBarRplIds;

	//------------------------------------------------------------------------------------------------
	void EL_SetQuickBarItems(array<RplId> quickBarRplIds)
	{
		Rpc(Rpc_SetQuickBarItems, quickBarRplIds);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void Rpc_SetQuickBarItems(array<RplId> quickBarRplIds)
	{
		EL_m_aQuickBarRplIds = quickBarRplIds;
	}

	//------------------------------------------------------------------------------------------------
	override protected void RpcAsk_NotifyOnPlayerSpawned()
	{
		super.RpcAsk_NotifyOnPlayerSpawned();

		// TODO: Refactor after 0.9.9's respawn system changes and make sure the controlled entity is known as early as it should.
		SCR_PlayerController playerController = SCR_PlayerController.Cast(m_PlayerController);
		if (playerController)
			playerController.m_OnControlledEntityChanged.Insert(EL_OnControlledEntityChanged);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_OnControlledEntityChanged(IEntity from, IEntity to)
	{
		// Postpone to next frame because otherwide not all RplId's are known already.
		GetGame().GetCallqueue().Call(EL_ApplyQuickbar);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void EL_ApplyQuickbar()
	{
		SCR_CharacterInventoryStorageComponent inventoryStorage = EL_Component<SCR_CharacterInventoryStorageComponent>.Find(m_PlayerController.GetControlledEntity());
		if (inventoryStorage)
		{
			inventoryStorage.EL_Rpc_UpdateQuickSlotItems(EL_m_aQuickBarRplIds);
			EL_m_aQuickBarRplIds = null;
		}
	}
};
