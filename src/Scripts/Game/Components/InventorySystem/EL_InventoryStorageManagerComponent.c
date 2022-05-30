modded class SCR_InventoryStorageManagerComponent
{
	override void InsertItem(IEntity pItem, BaseInventoryStorageComponent pStorageTo = null, BaseInventoryStorageComponent pStorageFrom = null, SCR_InvCallBack cb = null)
	{
		if (!pStorageTo)
		{
			super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
			return;
		}

		IEntity owner = pStorageTo.GetOwner();

		auto trader = EL_TraderManagerComponent.Cast(owner.FindComponent(EL_TraderManagerComponent));
		if (trader)
		{
			// check if we get the correct Item traded
			if (pItem.GetPrefabData().GetPrefabName() != trader.m_ItemToReceive)
			{
				this.SetReturnCode(EInventoryRetCode.RETCODE_ITEM_TOO_BIG); // Todo check how we can improve this
				cb.InvokeOnFailed();
				return;
			}
			
			// delete Item that we get traded
			bool deleteSuccess = this.TryDeleteItem(pItem);
			if (!deleteSuccess){
				cb.InvokeOnFailed();
				return;
			}
			// spawn and insert ItemToGive into inventory
			auto barterItem = GetGame().SpawnEntityPrefab(Resource.Load(trader.m_ItemToGive));
			bool insertSuccess = TryInsertItemInStorage(barterItem, pStorageFrom);
			if (!insertSuccess){
				TryInsertItemInStorage(GetGame().SpawnEntityPrefab(Resource.Load(trader.m_ItemToReceive)), pStorageFrom);
				return;
			}
			this.SetReturnCode(EInventoryRetCode.RETCODE_OK);
			cb.InvokeOnComplete();
			return;
		}

		//! TODO: Handle if the item was dragged directly onto the existing item or next to it. If next to it, it doesn't combine.

		EL_InventoryQuantityComponent quantityComponent = EL_InventoryQuantityComponent.Cast(pItem.FindComponent(EL_InventoryQuantityComponent));
		if (quantityComponent)
		{
			if (!pItem || !IsAnimationReady() || IsInventoryLocked())
			{
				return;
			}

			SetInventoryLocked(true);

			EL_InventoryQuantityComponent targetQuantityComponent = EL_FindEntityToCombineWith(quantityComponent, pStorageTo);
			if (targetQuantityComponent)
			{
				targetQuantityComponent.Combine(quantityComponent, this);
				
				SetInventoryLocked(false);

				//! Assuming everything is OK does not damage anything
				SetReturnCode(EInventoryRetCode.RETCODE_OK);
				cb.InvokeOnComplete();
				
				return;
			}

			SetInventoryLocked(false);

			//! There is no entity to combine with
			
			super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
			return;
		}

		super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
	}

	protected EL_InventoryQuantityComponent EL_FindEntityToCombineWith(EL_InventoryQuantityComponent pQuantity, BaseInventoryStorageComponent pStorage)
	{
		if (!pQuantity || !pQuantity.IsValid())
		{
			return null;
		}

		int count = pStorage.GetSlotsCount();
		if (count == 0)
		{
			return null;
		}

		IEntity entity = pQuantity.GetOwner();
		if (!entity)
		{
			return null;
		}

		EntityPrefabData entityPrefab = entity.GetPrefabData();
		if (!entityPrefab)
		{
			return null;
		}

		for (int i = 0; i < count; i++)
		{
			IEntity otherItem = pStorage.Get(i);

			//! Should be redundant
			if (!otherItem)
			{
				continue;
			}

			//! The item we will combine with is of the same type
			if (entityPrefab != otherItem.GetPrefabData())
			{
				continue;
			}

			EL_InventoryQuantityComponent quantityComponent = EL_InventoryQuantityComponent.Cast(otherItem.FindComponent(EL_InventoryQuantityComponent));
			if (!quantityComponent || !quantityComponent.IsValid())
			{
				continue;
			}
			
			bool canCombine = quantityComponent.CanCombine(pQuantity);
			if (!canCombine)
			{
				continue;
			}

			return quantityComponent;
		}

		return null;
	}

	void EL_Combine(EL_InventoryQuantityComponent itemA, EL_InventoryQuantityComponent itemB)
	{
		Rpc(RPC_EL_Combine, Replication.FindId(itemA), Replication.FindId(itemB));
	}

	void EL_Split(EL_InventoryQuantityComponent item, BaseInventoryStorageComponent destination)
	{
		int quantity = item.GetQuantity();
		int quantityA = Math.Floor(0.5 * quantity);
		int quantityB = Math.Ceil(0.5 * quantity);

		//! If the destination is the same and the quantity doesn't change then early terminate
		if (destination == item.GetOwningStorage() && (quantityA == 0 || quantityB == 0))
		{
			return;
		}

		RplId itemId = Replication.FindId(item);
		RplId destinationId = Replication.FindId(destination);
		Rpc(RPC_EL_Split, itemId, destinationId, quantityA, quantityB);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_Combine(RplId itemAId, RplId itemBId)
	{
		EL_InventoryQuantityComponent itemA = EL_InventoryQuantityComponent.Cast(Replication.FindItem(itemAId));
		if (!itemA)
		{
			return;
		}

		EL_InventoryQuantityComponent itemB = EL_InventoryQuantityComponent.Cast(Replication.FindItem(itemBId));
		if (!itemB)
		{
			return;
		}

		itemA.LocalCombine(itemB);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_Split(RplId itemId, RplId destinationId, int quantityA, int quantityB)
	{
		EL_InventoryQuantityComponent item = EL_InventoryQuantityComponent.Cast(Replication.FindItem(itemId));
		if (!item)
		{
			return;
		}

		BaseInventoryStorageComponent destination = BaseInventoryStorageComponent.Cast(Replication.FindItem(destinationId));
		if (!destination)
		{
			return;
		}

		item.LocalSplit(destination, this, quantityA, quantityB);
	}
}

modded class SCR_InvCallBack
{
	void InvokeOnComplete()
	{
		this.OnComplete();
	}

	void InvokeOnFailed()
	{
		this.OnFailed();
	}
};
