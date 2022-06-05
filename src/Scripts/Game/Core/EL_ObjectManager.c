class EL_ObjectManager
{
	//Subscribe to object creation events triggered by game mode logic
	static const ref ScriptInvoker Event_OnEntityCreated = new ScriptInvoker;

	static IEntity SpawnEntity(typename typeName, BaseWorld world = null, EntitySpawnParams params = null)
	{
		IEntity entity = GetGame().SpawnEntity(typeName, world, params);
		
		if(entity)
		{
			Event_OnEntityCreated.Invoke(entity);
		}
		
		return entity;
	}

	static IEntity SpawnEntityPrefab(notnull Resource templateResource, BaseWorld world = null, EntitySpawnParams params = null)
	{
		IEntity entity = GetGame().SpawnEntityPrefab(templateResource, world, params);
		
		if(entity)
		{
			Event_OnEntityCreated.Invoke(entity);
		}
		
		return entity;
	}

    static IEntity SpawnEntityPrefabLocal(notnull Resource templateResource, BaseWorld world = null, EntitySpawnParams params = null)
	{
		IEntity entity = GetGame().SpawnEntityPrefabLocal(templateResource, world, params);
		
		if(entity)
		{
			Event_OnEntityCreated.Invoke(entity);
		}
		
		return entity;
	}
};
