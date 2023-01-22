class EL_NetworkUtils
{
	//------------------------------------------------------------------------------------------------
	static RplId GetRplId(IEntity entity)
	{
		if (entity)
		{
			RplComponent replication = RplComponent.Cast(entity.FindComponent(RplComponent));
			if (replication) return replication.Id();
		}

		return RplId.Invalid();
	}

	//------------------------------------------------------------------------------------------------
	//! Finds an entity by its repliction id
	//! \param rplId Replication id to search for
	//! \return the the entity found or null if not found or invalid replication id
	static IEntity FindEntityByRplId(RplId rplId)
	{
		IEntity entity = null;

		if (rplId.IsValid())
		{
			RplComponent entityRpl = RplComponent.Cast(Replication.FindItem(rplId));
			if (entityRpl) entity = IEntity.Cast(entityRpl.GetEntity());
		}

		return entity;
	}

	//------------------------------------------------------------------------------------------------
	static bool IsOwner(IEntity entity)
	{
		if (!entity) return false;
		RplComponent replication = RplComponent.Cast(entity.FindComponent(RplComponent));
		if (!replication) return false;
		return replication.IsOwner();
	}

	//------------------------------------------------------------------------------------------------
	static bool IsMaster(IEntity entity)
	{
		if (!entity) return false;
		RplComponent replication = RplComponent.Cast(entity.FindComponent(RplComponent));
		if (!replication) return false;
		return replication.IsMaster();
	}
	
	//------------------------------------------------------------------------------------------------
	static EL_RpcSenderComponent GetLocalRpcSender()
	{
		return EL_ComponentFinder<EL_RpcSenderComponent>.Find(SCR_PlayerController.GetLocalControlledEntity());
	}
}
