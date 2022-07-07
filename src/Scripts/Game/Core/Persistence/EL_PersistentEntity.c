class EL_PersistentComponent
{
	//Always belong to an owner entity so they do not need their own id
}

class EL_PersistentEntity : EL_DbEntity
{
	ResourceName m_Prefab;
	
	ref array<ref EL_PersistentComponent> m_Components;
	
	static EL_PersistentEntity From(IEntity worldEntity)
	{
	}
	
	IEntity Spawn()
	{
	}
}
