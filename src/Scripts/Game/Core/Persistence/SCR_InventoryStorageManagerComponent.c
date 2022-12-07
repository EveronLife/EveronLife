modded class ScriptedInventoryStorageManagerComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemAdded(storageOwner, item);
		EL_PersistenceComponent persistenceComponent = EL_ComponentFinder<EL_PersistenceComponent>.Find(item);
		if (persistenceComponent) persistenceComponent.OnStorageParentChanged(item, storageOwner.GetOwner());
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemRemoved(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemRemoved(storageOwner, item);
		EL_PersistenceComponent persistenceComponent = EL_ComponentFinder<EL_PersistenceComponent>.Find(item);
		if (persistenceComponent) persistenceComponent.OnStorageParentChanged(item, null);
	}
}
