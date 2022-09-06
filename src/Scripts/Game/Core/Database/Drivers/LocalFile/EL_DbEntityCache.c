class EL_DbEntityCache
{
	ref map<string, ref EL_DbEntity> m_EntityInstances;

	// TODO sorted linked list with last queried for, drop by id from cache if not asked for in X time

	//------------------------------------------------------------------------------------------------
	void Add(EL_DbEntity entity)
	{
		m_EntityInstances.Set(entity.GetId(), entity);
	}

	//------------------------------------------------------------------------------------------------
	void Remove(string entityId)
	{
		m_EntityInstances.Remove(entityId);
	}


	EL_DbEntity Get(string entityId)
	{
		return m_EntityInstances.Get(entityId);
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbEntityCache()
	{
		m_EntityInstances = new map<string, ref EL_DbEntity>();
	}
}
