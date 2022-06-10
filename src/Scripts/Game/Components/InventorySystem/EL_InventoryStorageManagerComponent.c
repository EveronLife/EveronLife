modded class SCR_InventoryStorageManagerComponent
{
	override void InsertItem(IEntity pItem, BaseInventoryStorageComponent pStorageTo = null, BaseInventoryStorageComponent pStorageFrom = null, SCR_InvCallBack cb = null)
	{
		if (TryCombineItems(pItem, pStorageTo, pStorageFrom, cb))
		{
			return;
		}

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

		super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
	}

	bool TryCombineItems(IEntity pItem, BaseInventoryStorageComponent pStorageTo, BaseInventoryStorageComponent pStorageFrom, SCR_InvCallBack cb)
	{
		EL_InventoryQuantityComponent quantityComponent = EL_InventoryQuantityComponent.Cast(pItem.FindComponent(EL_InventoryQuantityComponent));
		if (!quantityComponent)
		{
			return false;
		}

		if (pStorageTo)
		{
			return false;
		}

		EL_InventoryQuantityComponent targetQuantityComponent = EL_FindEntityToCombineWith(quantityComponent, pStorageTo);
		if (!targetQuantityComponent)
		{
			return false;
		}

		targetQuantityComponent.Combine(quantityComponent, this);
		return true;
	}

	override bool EquipAny(BaseInventoryStorageComponent storage, IEntity item, int prefered = 0, SCR_InvCallBack cb = null)
	{
		if (!cb || !cb.m_pMenu)
		{
			return super.EquipAny(storage, item, prefered, cb);
		}

		EL_InventoryQuantityComponent quantityComponent = EL_InventoryQuantityComponent.Cast(item.FindComponent(EL_InventoryQuantityComponent));
		if (!quantityComponent)
		{
			return super.EquipAny(storage, item, prefered, cb);
		}
		
		SCR_InventorySlotUI slotUi = cb.m_pMenu.EL_GetFocusedSlotUI();
		if (!slotUi)
		{
			return super.EquipAny(storage, item, prefered, cb);
		}

		InventoryItemComponent targetItemComponent = slotUi.GetItem();
		if (!targetItemComponent)
		{
			return super.EquipAny(storage, item, prefered, cb);
		}
		
		IEntity targetItem = targetItemComponent.GetOwner();
		if (!targetItem)
		{
			return super.EquipAny(storage, item, prefered, cb);
		}
		
		EL_InventoryQuantityComponent targetQuantityComponent = EL_InventoryQuantityComponent.Cast(targetItem.FindComponent(EL_InventoryQuantityComponent));
		if (!targetQuantityComponent)
		{
			return super.EquipAny(storage, item, prefered, cb);
		}

		if (!targetQuantityComponent.CanCombine(quantityComponent))
		{
			return super.EquipAny(storage, item, prefered, cb);
		}

		targetQuantityComponent.Combine(quantityComponent, this);
		return true;
	}

	protected EL_InventoryQuantityComponent EL_FindEntityToCombineWith(EL_InventoryQuantityComponent pQuantity, BaseInventoryStorageComponent pStorage)
	{
		if (!pQuantity || !pQuantity.IsValid())
		{
			return null;
		}

		array<IEntity> items();
		int count = 0;

		if (pStorage)
		{
			pStorage.GetAll(items);
			count = items.Count();
		}
		else
		{
			EL_InventoryQuantityPredicate predicate = new EL_InventoryQuantityPredicate();
			count = FindItems(items, predicate, EStoragePurpose.PURPOSE_ANY);
		}

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
			IEntity otherItem = items.Get(i);

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
			
			if (!quantityComponent.CanCombine(pQuantity, true))
			{
				continue;
			}

			return quantityComponent;
		}

		return null;
	}

	void EL_Combine(EL_InventoryQuantityComponent itemA, EL_InventoryQuantityComponent itemB)
	{
		Rpc(EL_RPC_Combine, Replication.FindId(itemA), Replication.FindId(itemB));
	}

	void EL_Split(EL_InventoryQuantityComponent itemA, float split)
	{
		int quantity = itemA.GetQuantity();
		int quantityA = Math.Floor(quantity * split);
		int quantityB = Math.Ceil(quantity * (1.0 - split));

		//! If the quantity doesn't change then early terminate
		if (quantityA == 0 || quantityB == 0)
		{
			return;
		}

		BaseInventoryStorageComponent storage = itemA.GetOwningStorage();
		if (storage)
		{
			IEntity entity = itemA.GetOwner();
			EntityPrefabData entityData = entity.GetPrefabData();
			
			ResourceName resourceName = entityData.GetPrefabName();
			TrySpawnPrefabToStorage(resourceName, storage);
		}
		
		RplId itemAId = Replication.FindId(itemA);
		
		Rpc(EL_RPC_Split, itemAId, split);
	}

	//! RPCs must be performed in the StorageManagerComponent as that is an owner on client so can send to the server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void EL_RPC_Combine(RplId itemAId, RplId itemBId)
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
	protected void EL_RPC_Split(RplId itemAId, float split)
	{
		EL_InventoryQuantityComponent itemA = EL_InventoryQuantityComponent.Cast(Replication.FindItem(itemAId));
		if (!itemA)
		{
			return;
		}

		BaseInventoryStorageComponent storage = itemA.GetOwningStorage();
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (storage && !rpl.IsMaster()) //! Spawned on client first then server. Master check for "host"
		{
			IEntity entity = itemA.GetOwner();
			EntityPrefabData entityData = entity.GetPrefabData();
			
			ResourceName resourceName = entityData.GetPrefabName();
			TrySpawnPrefabToStorage(resourceName, storage);
		}
		else if (!storage) //! Spawned on server regardless if there is no specified storage
		{
			IEntity entity = itemA.GetOwner();
			EntityPrefabData entityData = entity.GetPrefabData();

			ResourceName resourceName = entityData.GetPrefabName();
			Resource resource = Resource.Load(resourceName);
			if (!resource.IsValid())
			{
				return;
			}

			EntitySpawnParams params();
			
			//! Owner is player
			GetOwner().GetWorldTransform(params.Transform);

			GetGame().SpawnEntityPrefab(resource, entity.GetWorld(), params);
		}
		
		EL_InventoryQuantityComponent itemB = EL_InventoryQuantityComponent.GetLastCreated();

		itemA.LocalSplit(itemB, this, split);
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
