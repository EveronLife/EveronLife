modded class SCR_CharacterInventoryStorageComponent
{
	//------------------------------------------------------------------------------------------------
	override void StoreItemToQuickSlot(notnull IEntity pItem, int iSlotIndex = -1, bool isForced = false)
	{
		super.StoreItemToQuickSlot(pItem, iSlotIndex, isForced);
		
		// Debounce sync to 30 seconds after the last change.
		RplComponent replication = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if(replication && replication.IsOwner())
		{
			GetGame().GetCallqueue().Remove(EL_SyncQuickSlots);
			GetGame().GetCallqueue().CallLater(EL_SyncQuickSlots, 30000);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void EL_SyncQuickSlots()
	{
		array<RplId> rplIds();
		rplIds.Resize(m_aQuickSlots.Count());
		
		foreach (int idx, IEntity quickSlotItem : m_aQuickSlots)
		{
			RplId rplId = RplId.Invalid();
			
			if (quickSlotItem)
			{
				RplComponent replication = RplComponent.Cast(quickSlotItem.FindComponent(RplComponent));
				if(replication) rplId = replication.Id();
			}
			
			rplIds.Set(idx, rplId);
		}
		
		Rpc(EL_Rpc_UpdateQuickSlotItems, rplIds);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void EL_Rpc_UpdateQuickSlotItems(array<RplId> rplIds)
	{
		if (!rplIds || (m_aQuickSlots.Count() != rplIds.Count())) return;
		
		// Dequeue any pending update if we just received data from server
		RplComponent replication = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if(replication && replication.IsOwner())
		{
			GetGame().GetCallqueue().Remove(EL_SyncQuickSlots);
		}
		
		int slotsCount = m_aDefaultQuickSlots.Count();	
		if (m_aQuickSlotsHistory.Count() < slotsCount)
		{
			m_aQuickSlotsHistory.Resize(slotsCount);
		}
		
		foreach (int idx, RplId rplId : rplIds)
		{
			IEntity slotEntity = EL_Utils.FindEntityByRplId(rplId);
			m_aQuickSlots.Set(idx, slotEntity);
			if (slotEntity) m_aQuickSlotsHistory.Set(idx, GetItemType(slotEntity));		
		}
	}
}
