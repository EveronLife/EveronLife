enum EL_PersistenceManagerState
{
	WORLD_INIT,
	ACTIVE,
	SHUTDOWN
}

class EL_PersistenceManager
{
	protected static ref EL_PersistenceManager s_pInstance;
	
	protected EL_PersistenceManagerState m_eState;
	
	EL_PersistenceManagerState GetState()
	{
		return m_eState;
	}
	
	void StartTacking(Class trackableInstance)
	{
		
	}
	
	void StopTacking(Class trackableInstance)
	{
		//can not delete entries on stop tracking instantly, cause it would mess up the auto save state.
		//extra function to "removelater" for autosave (which is triggered just before shutdown?) to delete world objects etc together with new being created etc
		//if object still exists but is not toplevel anymore, then its not tracked but not deleted either.
		
		//no extra function, on stop tracking get the id for it and the instance as weak ref. on next autosave check if it has become null, and mass remove all that were deleted.
		//for scripted states aka not inherit from IEntity we need to make a mapping in this manager to associate instance with an ID anyway :)
	}
	
	void SaveAll()
	{
		
	}
	
	void OnWorldPostProcess(World world)
	{
		m_eState = EL_PersistenceManagerState.ACTIVE;
	}
	
	void OnGameEnd()
	{
		m_eState = EL_PersistenceManagerState.SHUTDOWN;
		
		//Flush changes into persistency -> TODO: How to best make this blocking?
		SaveAll();
		
		//Cleanup after end of current session
		Reset();
	}
	
	/*
	void OnPostFrame(float timeSlice)
	{
		Print(timeSlice);
	}
	*/
	
	static EL_PersistenceManager GetInstance()
	{
		if(!s_pInstance)
		{
			s_pInstance = new EL_PersistenceManager();
			
			//Reset the singleton when a new mission is loaded to free all memory and have a clean startup again.
			GetGame().m_OnMissionSetInvoker.Insert(Reset);
		}
		
		return s_pInstance;
	}
	
	protected static void Reset()
	{
		s_pInstance = null;
	}
	
	protected void EL_PersistenceManager()
	{
		m_eState = EL_PersistenceManagerState.WORLD_INIT;
	}
}
