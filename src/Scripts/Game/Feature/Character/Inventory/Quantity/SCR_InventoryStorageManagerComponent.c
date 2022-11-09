modded class SCR_InventoryStorageManagerComponent
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
	void EL_RequestQuantitySplit(notnull IEntity sourceEntity, int splitSize = -1)
	{
		Rpc(RPC_EL_RequestQuantitySplit, EL_NetworkUtils.GetRplId(sourceEntity), splitSize);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_RequestQuantitySplit(RplId quantitySourceRplId, int splitSize)
	{
		EL_QuantityComponent quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(EL_NetworkUtils.FindEntityByRplId(quantitySourceRplId));
		if (!quantitySource || !EL_CanManipulate(quantitySource.GetOwner())) return;
		quantitySource.Split(splitSize);
	}

	//------------------------------------------------------------------------------------------------
	void EL_RequestQuantityTransfer(notnull IEntity sourceEntity, notnull IEntity destinationEntity, int amount = -1, bool setQuantity = false)
	{
		Rpc(RPC_EL_QuantityTransfer, EL_NetworkUtils.GetRplId(sourceEntity), EL_NetworkUtils.GetRplId(destinationEntity), amount, setQuantity);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_QuantityTransfer(RplId quantitySourceRplId, RplId quantityDestinationRplId, int amount, bool setQuantity)
	{
		EL_QuantityComponent quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(EL_NetworkUtils.FindEntityByRplId(quantitySourceRplId));
		if (!quantitySource || !EL_CanManipulate(quantitySource.GetOwner())) return;

		EL_QuantityComponent quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(EL_NetworkUtils.FindEntityByRplId(quantityDestinationRplId));
		if (!quantityDestination || !EL_CanManipulate(quantityDestination.GetOwner())) return;

		quantityDestination.Combine(quantitySource, amount, setQuantity);
	}

	//------------------------------------------------------------------------------------------------
	void EL_SetTransferIntent(notnull IEntity sourceEntity, bool keepSeperate)
	{
		Rpc(RPC_EL_SetTransferIntent, EL_NetworkUtils.GetRplId(sourceEntity), keepSeperate);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_SetTransferIntent(RplId sourceRplId, bool keepSeperate)
	{
		IEntity sourceEntity = EL_NetworkUtils.FindEntityByRplId(sourceRplId);
		if (!sourceEntity || !EL_CanManipulate(sourceEntity)) return;
		EL_QuantityComponent.SetTransferIntent(sourceEntity, keepSeperate);
	}

	//------------------------------------------------------------------------------------------------
	protected bool EL_CanManipulate(IEntity entity)
	{
		const int maxManipulationRange = 10;
		return (vector.Distance(entity.GetOrigin(), GetOwner().GetOrigin()) < maxManipulationRange);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		if (EL_QuantityComponent.HandleOnItemAdded(this, storageOwner, item)) return;
		super.OnItemAdded(storageOwner, item);
	}

	//------------------------------------------------------------------------------------------------
	override void PlayItemSound(IEntity entity, string soundEvent)
	{
		// TODO: Needed until https://feedback.bistudio.com/T167936 is fixed, because item is merged on quantity pickup auto combine
		if (entity) super.PlayItemSound(entity, soundEvent)
	}
}
