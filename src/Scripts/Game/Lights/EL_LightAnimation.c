/**
 Manages the entries of the animation
**/
[BaseContainerProps()]
class EL_LightAnimation
{
	[Attribute(desc: "Entries that will be executed in order when the mode is selected")]
	protected ref array<ref EL_BaseUserEntry> m_Entries;
	
	protected ref EL_UpdateLVEntry m_LVUpdater = new EL_UpdateLVEntry();
	
	// When it is 0 or less, it executes entries
	protected float m_Timer = 0;
	// Holds the current state of the animation
	protected float m_TimeInLoop = 0;
	
	// Current entry being executed
	protected int m_EntryIndex = 0;
	
	// For each Tick() the animation will Simulate() m_Precision times
	protected int m_Precision = 1;
	
	//------------------------------------------------------------------------------------------------
	/**
 	\brief inserts the light into each entry
	\param light - light being inserted
	**/
	void InsertLight(EL_SirenLightComponent light)
	{
		foreach(EL_BaseEntry entry : m_Entries)
		{
			entry.OnRegister(light);
		}
		m_LVUpdater.OnRegister(light);
	}
	
	//------------------------------------------------------------------------------------------------
	/**
 	\brief Delays the next entry execution
	\param delay - delay in seconds
	**/
	void SetDelay(float delay)
	{
		m_Timer += delay;
	}
	
	//------------------------------------------------------------------------------------------------
	/**
 	\brief Rewinds a number of steps in the animation
	\param steps - the number of entries that should be rewinded
	**/
	void GoBack(int steps)
	{
		int nextIdx = m_EntryIndex - steps;
		if(nextIdx <= 0) Reset();
		else m_EntryIndex = nextIdx;
	}
	
	//------------------------------------------------------------------------------------------------
	/**
 	\brief resets the state of the animation
	**/
	void Reset()
	{
		m_EntryIndex = 0;
		m_TimeInLoop = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	/**
 	\return the time spent in the current loop of the animation
	**/
	float GetTimeInLoop()
	{
		return m_TimeInLoop;
	}
	
	//------------------------------------------------------------------------------------------------
	/**
 	\brief executes all entries until the end or until m_Timer is greater than 0
	\param timeSlice - the time step in seconds
	**/
	protected void Simulate(float timeSlice)
	{
		m_Timer -= timeSlice;
		m_TimeInLoop += timeSlice;
		while(m_Timer <= 0 && m_EntryIndex < m_Entries.Count()) 
		{
			m_Entries[m_EntryIndex].Execute(this);
			m_EntryIndex++;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/**
	\brief If the simulation hasnt reached the end, simulates the animation m_Precision times
	\param timeSlice - time in seconds to be simulated
	**/
	void Tick(float timeSlice)
	{
		m_LVUpdater.OnTick(timeSlice);
		if(m_EntryIndex < m_Entries.Count())
		{
			float step = timeSlice / m_Precision;
			for(int i = 0; i < m_Precision; i++)
			{
				if(m_EntryIndex < m_Entries.Count())
					Simulate(step);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPrecision(int precision)
	{
		if(precision < 1) m_Precision = 1;
		else m_Precision = precision;
	}
	
}
