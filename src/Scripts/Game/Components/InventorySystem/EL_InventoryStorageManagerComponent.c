modded class SCR_InventoryStorageManagerComponent
{
	override void InsertItem( IEntity pItem, BaseInventoryStorageComponent pStorageTo = null, BaseInventoryStorageComponent pStorageFrom = null, SCR_InvCallBack cb = null  )
	{
		if(pStorageTo) {
			IEntity owner = pStorageTo.GetOwner();
			auto trader = EL_TraderManagerComponent.Cast(owner.FindComponent(EL_TraderManagerComponent));
			if (trader) {
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
		}
		super.InsertItem(pItem, pStorageTo, pStorageFrom, cb);
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