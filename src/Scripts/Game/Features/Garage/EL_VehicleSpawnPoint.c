class EL_VehicleSpawnPointClass : ScriptComponentClass
{
}

class EL_VehicleSpawnPoint : ScriptComponent
{
	private BaseGameTriggerEntity m_SpawnAreaTrigger;

	//------------------------------------------------------------------------------------------------
	bool IsFree()
	{
		array<IEntity> entInTrigger = new array<IEntity>();
		m_SpawnAreaTrigger.QueryEntitiesInside();
		m_SpawnAreaTrigger.GetEntitiesInside(entInTrigger);

		return (entInTrigger.Count() == 0);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_SpawnAreaTrigger = BaseGameTriggerEntity.Cast(owner);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
}
