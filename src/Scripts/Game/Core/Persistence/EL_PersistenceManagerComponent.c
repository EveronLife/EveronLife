[ComponentEditorProps(category: "EveronLife/Core/Persistence", description: "Must be attached to the gamemode entity to setup the persistence system.")]
class EL_PersistenceManagerComponentClass : SCR_BaseGameModeComponentClass
{
	[Attribute(defvalue: "1", desc: "Enable or disable auto-save globally", category: "Auto-Save")]
	bool m_bEnableAutosave;

	[Attribute(defvalue: "600", desc: "Time between auto-save in seconds.", category: "Auto-Save")]
	float m_fAutosaveInterval;

	[Attribute(defvalue: "5", uiwidget: UIWidgets.Slider, desc: "Maximum number of entities processed during a single update tick.", params: "1 128 1", category: "Auto-Save")]
	int m_iAutosaveIterations;

	[Attribute(defvalue: "0.33", uiwidget: UIWidgets.Slider, desc: "Adjust the tick rate of the persistence manager", params: "0.01 10 0.01", category: "Advanced", precision: 2)]
	float m_fUpdateRate;

	[Attribute(defvalue: "JsonFile://EveronLife?pretty=true", desc: "Default database connection string. Can be overriden using \"-ConnectionString=...\" CLI argument", category: "Database")]
	string m_sDatabaseConnectionString;
};

class EL_PersistenceManagerComponent : SCR_BaseGameModeComponent
{
	protected EL_PersistenceManager m_pPersistenceManager;
	protected float m_fAccumulator;
	protected float m_fUpdateRateSetting;

	//------------------------------------------------------------------------------------------------
	override event void OnWorldPostProcess(World world)
	{
		super.OnWorldPostProcess(world);
		if (m_pPersistenceManager)
			m_pPersistenceManager.OnWorldPostProcess(world);
	}

	//------------------------------------------------------------------------------------------------
	override event void OnGameEnd()
	{
		super.OnGameEnd();
		if (m_pPersistenceManager)
			m_pPersistenceManager.OnGameEnd();
	}

	//------------------------------------------------------------------------------------------------
	override event void EOnPostFrame(IEntity owner, float timeSlice)
	{
		super.EOnPostFrame(owner, timeSlice);
		m_fAccumulator += timeSlice;
		if (m_fAccumulator >= m_fUpdateRateSetting)
		{
			m_pPersistenceManager.OnPostFrame(m_fAccumulator);
			m_fAccumulator = 0;
		}
	}

	//------------------------------------------------------------------------------------------------
	override event void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		// Persistence logic only runs on the server machine
		if (!EL_PersistenceManager.IsPersistenceMaster())
			return;

		EL_PersistenceManagerComponentClass settings = EL_PersistenceManagerComponentClass.Cast(GetComponentData(owner));
		if (settings.m_fAutosaveInterval < settings.m_fUpdateRate)
		{
			Debug.Error(string.Format("Update rate '%1' must be smaller than auto-save interval '%2'.", settings.m_fUpdateRate, settings.m_fAutosaveInterval));
			return;
		}

		string connectionStringOverride;
		if (System.GetCLIParam("ConnectionString", connectionStringOverride))
			settings.m_sDatabaseConnectionString = connectionStringOverride;

		m_fUpdateRateSetting = settings.m_fUpdateRate;
		m_pPersistenceManager = EL_PersistenceManager.GetInstance();
		m_pPersistenceManager.OnPostInit(owner, settings);
		SetEventMask(owner, EntityEvent.POSTFRAME);
	}
};
