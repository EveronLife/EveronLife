modded class SCR_VehicleInventoryStorageManagerComponent
{
	//------------------------------------------------------------------------------------------------
	//! TODO: Remove after serverside insert works (likely 0.9.6 or 0.9.7 patch)
	void EL_RequestInsertItemInStorage(notnull IEntity item, notnull BaseInventoryStorageComponent storage)
	{
		array<Managed> outComponents();
		storage.GetOwner().FindComponents(BaseInventoryStorageComponent, outComponents);
		foreach (int idx, Managed componentRef : outComponents)
		{
			if (componentRef == storage)
			{
				Rpc(RPC_EL_RequestInsertItemInStorage, EL_NetworkUtils.GetRplId(item), EL_NetworkUtils.GetRplId(storage.GetOwner()), idx);
				break;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	//! TODO: Remove after serverside insert works (likely 0.9.6 or 0.9.7 patch)
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RPC_EL_RequestInsertItemInStorage(RplId itemRplId, RplId storageOwnerRplId, int storageIdx)
	{
		IEntity item = EL_NetworkUtils.FindEntityByRplId(itemRplId);
		if (item)
		{
			IEntity storageOwner = EL_NetworkUtils.FindEntityByRplId(storageOwnerRplId);
			if (!storageOwner) return;
			array<Managed> outComponents();
			storageOwner.FindComponents(BaseInventoryStorageComponent, outComponents);
			if (storageIdx >= outComponents.Count()) return; //Idx out of bounds
			BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(outComponents[storageIdx]);
			TryInsertItemInStorage(item, storage);
			return;
		}

		// Try again to find it after replication for the item is completed
		GetGame().GetCallqueue().CallLater(RPC_EL_RequestInsertItemInStorage, 64, false, itemRplId, storageOwnerRplId, storageIdx);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		if (EL_QuantityComponent.HandleOnItemAdded(this, storageOwner, item)) return;
		super.OnItemAdded(storageOwner, item);
	}
}
