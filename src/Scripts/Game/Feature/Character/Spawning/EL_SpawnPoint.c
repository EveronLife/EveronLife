[EntityEditorProps(category: "EveronLife/Feature/Character/Spawning", description: "Character spawn point")]
class EL_SpawnPointClass : SCR_PositionClass
{
}

class EL_SpawnPoint : SCR_Position
{
	[Attribute("0", desc: "Find empty position for spawning within given radius. When none is found, entity position will be used.")]
	protected float m_fSpawnRadius;

	// TODO: Add bool for default spawn point when we have multiple?

	protected static ref array<EL_SpawnPoint> s_aSpawnPoints = new ref array<EL_SpawnPoint>();

	//------------------------------------------------------------------------------------------------
	static EL_SpawnPoint GetDefaultSpawnPoint()
	{
		if (s_aSpawnPoints.IsEmpty()) return null;
		return s_aSpawnPoints.Get(0);
	}

	//------------------------------------------------------------------------------------------------
	static array<EL_SpawnPoint> GetSpawnPoints()
	{
		return s_aSpawnPoints;
	}

	//------------------------------------------------------------------------------------------------
	#ifdef WORKBENCH
	override void SetColorAndText()
	{
		m_sText = "Global Spawn";
		m_iColor = Color.CYAN;
	}
	#endif

	//------------------------------------------------------------------------------------------------
	void GetPosAngles(out vector position, out vector angles)
	{
		position = GetOrigin();
		angles = GetAngles();
		SCR_WorldTools.FindEmptyTerrainPosition(position, position, m_fSpawnRadius);
	}

	//------------------------------------------------------------------------------------------------
	void EL_SpawnPoint(IEntitySource src, IEntity parent)
	{
		SetFlags(EntityFlags.STATIC, true);

		if (GetGame().GetWorldEntity()) s_aSpawnPoints.Insert(this);
	}

	//------------------------------------------------------------------------------------------------
	void ~EL_SpawnPoint()
	{
		if (s_aSpawnPoints) s_aSpawnPoints.RemoveItem(this);
	}
}
