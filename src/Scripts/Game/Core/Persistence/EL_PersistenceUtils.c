class EL_PersistenceUtils
{
	static bool IsAttached(notnull IEntity entity)
	{
		IEntity parent = entity.GetParent();

		// No parent, but all other systems checked would result in it having a parent so we can stop early
		if (!parent) return false;

		// If inventory item then return if parent slot is null or not
		InventoryItemComponent inventoryItem = EL_ComponentFinder<InventoryItemComponent>.Find(entity);
		if (inventoryItem) return inventoryItem.GetParentSlot();

		// Check if stored inside any storage on the parent
		array<Managed> outComponents();
		parent.FindComponents(BaseInventoryStorageComponent, outComponents);
		foreach (Managed componentRef : outComponents)
		{
			if (BaseInventoryStorageComponent.Cast(componentRef).Contains(entity)) return true;
		}

		// Check if entity is attached to any of the parents slots
		SlotManagerComponent slotManager = EL_ComponentFinder<SlotManagerComponent>.Find(parent);
		if (slotManager)
		{
			array<EntitySlotInfo> outSlots();
			slotManager.GetSlotInfos(outSlots);
			foreach (EntitySlotInfo slot : outSlots)
			{
				if (slot.GetAttachedEntity() == entity) return true;
			}
		}

		// Check attachment slot components
		outComponents.Clear();
		parent.FindComponents(AttachmentSlotComponent, outComponents);
		foreach (Managed componentRef : outComponents)
		{
			if (AttachmentSlotComponent.Cast(componentRef).GetAttachedEntity() == entity) return true;
		}

		// Check magazines in case parent might be a weapon
		outComponents.Clear();
		parent.FindComponents(BaseMuzzleComponent, outComponents);
		foreach (Managed componentRef : outComponents)
		{
			if (BaseMuzzleComponent.Cast(componentRef).GetMagazine() == entity) return true;
		}

		return false;
	}
}
