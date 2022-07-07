class EL_PersistentStorageItem
{
	int m_SlotId;
	EL_PersistentEntity m_Entity;
}

class EL_PersistentStorage : EL_PersistentComponent
{
	ref array<ref EL_PersistentStorageItem> m_Items;
}
