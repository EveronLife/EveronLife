[ComponentEditorProps(category: "EveronLife/Core/Persistence", description: "Must be attached to the gamemode entity to setup the persistence system.")]
class EL_PersistenceManagerComponentClass : SCR_BaseGameModeComponentClass
{
	[Attribute(defvalue: "1", desc: "Enable or disable auto-save globally", category: "Auto-Save")]
	bool m_bEnabled;

	[Attribute(defvalue: "600", desc: "Time between auto-save in seconds.", category: "Auto-Save")]
	float m_fInterval;

	[Attribute(defvalue: "5", uiwidget: UIWidgets.Slider, desc: "Maximum number of entities processed during a single update tick.", params: "1 128 1", category: "Auto-Save")]
	int m_iIterations;

	[Attribute(defvalue: "0.33", uiwidget: UIWidgets.Slider, desc: "Distance between surface and camera on y-axis", params: "0.01 10 0.01", category: "Advanced", precision: 2)]
	float m_fUpdateRate;
}

class EL_PersistenceManagerComponent : SCR_BaseGameModeComponent
{
	protected float m_fAccumulator;
	protected float m_fUpdateRateSetting;

	//------------------------------------------------------------------------------------------------
	override event void OnWorldPostProcess(World world)
	{
		super.OnWorldPostProcess(world);

		if (!EL_PersistenceManager.IsPersistenceMaster()) return;

		EL_PersistenceManagerInternal.GetInternalInstance().OnWorldPostProcess(world);
	}

	//------------------------------------------------------------------------------------------------
	override event void OnGameEnd()
	{
		super.OnGameEnd();

		if (!EL_PersistenceManager.IsPersistenceMaster()) return;

		EL_PersistenceManagerInternal.GetInternalInstance().OnGameEnd();
	}

	//------------------------------------------------------------------------------------------------
	override event void EOnPostFrame(IEntity owner, float timeSlice)
	{
		super.EOnPostFrame(owner, timeSlice);

		m_fAccumulator += timeSlice;

		if (m_fAccumulator >= m_fUpdateRateSetting)
		{
			EL_PersistenceManagerInternal.GetInternalInstance().OnPostFrame(m_fAccumulator);
			m_fAccumulator = 0;
		}
	}

	//------------------------------------------------------------------------------------------------
	override event void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		// Persistence logic only runs on the server machine
		if (!EL_PersistenceManager.IsPersistenceMaster()) return;

		EL_PersistenceManagerComponentClass settings = EL_PersistenceManagerComponentClass.Cast(GetComponentData(owner));
		if (settings.m_fInterval < settings.m_fUpdateRate)
		{
			Debug.Error(string.Format("Update rate '%1' must be smaller than auto-save interval '%2'.", settings.m_fUpdateRate, settings.m_fInterval));
			return;
		}

		m_fUpdateRateSetting = settings.m_fUpdateRate;

		SetEventMask(owner, EntityEvent.POSTFRAME);

		EL_PersistenceManagerInternal.GetInternalInstance().OnPostInit(owner);
	}
}
