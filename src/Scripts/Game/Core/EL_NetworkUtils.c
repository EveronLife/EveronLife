class EL_NetworkUtils
{
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
}
