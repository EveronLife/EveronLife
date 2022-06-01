class EL_InventoryStorageAccessControlClass: ScriptComponentClass
{
};


class EL_InventoryStorageAccessControl : ScriptComponent
{
	
	[Attribute("Storage Locked")]
	bool m_IsLocked;
	
	bool IsLocked(IEntity entity) {
		return m_IsLocked;
	}
}