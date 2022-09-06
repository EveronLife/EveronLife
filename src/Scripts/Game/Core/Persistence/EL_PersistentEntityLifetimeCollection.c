[EL_DbName(EL_PersistentEntityLifetimeCollection, "EntityLifetimeCollection")]
class EL_PersistentEntityLifetimeCollection : EL_DbEntity
{
	protected ref map<string, IEntity> m_mTrackedEntites = new map<string, IEntity>();
	protected ref map<string, float> m_mLifetimes;

	//------------------------------------------------------------------------------------------------
	void Add(EL_PersistenceComponent persistenceComponent)
	{
		GarbageManager garbageManager = GetGame().GetGarbageManager();
		if (!garbageManager) return;

		float lifetime = -1;
		if (m_mLifetimes)
		{
			lifetime = m_mLifetimes.Get(persistenceComponent.GetPersistentId());
			if (lifetime == 0) lifetime = -1;
		}

		// Only remove characters with known lifetime (aka dead chars)
		if (!ChimeraCharacter.Cast(persistenceComponent.GetOwner()) || lifetime != -1)
		{
			garbageManager.Insert(persistenceComponent.GetOwner(), lifetime);
		}

		m_mTrackedEntites.Set(persistenceComponent.GetPersistentId(), persistenceComponent.GetOwner());
	}

	//------------------------------------------------------------------------------------------------
	void Remove(string persistentId)
	{
		GarbageManager garbageManager = GetGame().GetGarbageManager();
		if (!garbageManager) return;

		IEntity entity = m_mTrackedEntites.Get(persistentId);
		if (!entity) return;

		garbageManager.Withdraw(entity);
		m_mTrackedEntites.Remove(persistentId);
		if (m_mLifetimes) m_mLifetimes.Remove(persistentId);
	}

	//------------------------------------------------------------------------------------------------
	void Save(EL_DbContext dbContext)
	{
		GarbageManager garbageManager = GetGame().GetGarbageManager();

		if (garbageManager)
		{
			m_mLifetimes = new map<string, float>();

			PlayerManager playerManager = GetGame().GetPlayerManager();

			foreach (string persistentId, IEntity entity: m_mTrackedEntites)
			{
				// Do not allow controlled characters to be saved in GC
				if (!persistentId || !entity || playerManager.GetPlayerIdFromControlledEntity(entity)) continue;

				float lifeTime = garbageManager.GetLifetime(entity);
				if (lifeTime == -1) continue;

				m_mLifetimes.Set(persistentId, lifeTime);
			}
		}

		// Remove collection if it only holds default values
		if (!m_mLifetimes || m_mLifetimes.IsEmpty())
		{
			// Only need to call db if it was previously saved (aka it has an id)
			if (HasId()) dbContext.RemoveAsync(this);

			m_mLifetimes = null;
		}
		else
		{
			dbContext.AddOrUpdateAsync(this);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;

		saveContext.WriteValue("m_iDataLayoutVersion", 1);
		saveContext.WriteValue("m_sId", GetId());

		array<ref EL_PersistentEntityLifetime> lifetimes();
		lifetimes.Resize(m_mLifetimes.Count());

		int idx;
		foreach (string persistentId, float lifetime : m_mLifetimes)
		{
			EL_PersistentEntityLifetime persistentLifetime();
			persistentLifetime.m_sPersistenceId = persistentId;
			persistentLifetime.m_fLifetime = lifetime;
			lifetimes.Set(idx++, persistentLifetime);
		}

		saveContext.WriteValue("m_aLifetimes", lifetimes);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;

		int m_iDataLayoutVersion;
		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);

		string id;
		loadContext.ReadValue("m_sId", id);
		SetId(id);

		m_mLifetimes = new map<string, float>();

		array<ref EL_PersistentEntityLifetime> lifetimes();
		loadContext.ReadValue("m_aLifetimes", lifetimes);
		foreach (EL_PersistentEntityLifetime persistentLifetime : lifetimes)
		{
			m_mLifetimes.Set(persistentLifetime.m_sPersistenceId, persistentLifetime.m_fLifetime);
		}

		return true;
	}
}

class EL_PersistentEntityLifetime
{
	string m_sPersistenceId;
	float m_fLifetime;
}
