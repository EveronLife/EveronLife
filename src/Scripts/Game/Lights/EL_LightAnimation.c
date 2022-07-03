[BaseContainerProps()]
class EL_LightAnimation
{
	[Attribute(desc: "Entries that will be executed in order when the mode is selected")]
	protected ref array<ref EL_BaseEntry> m_Entries;
	
	protected float m_Timer = 0;
	
	protected int m_EntryIndex = 0;
	
	protected int m_Precision = 1;
	
	void Insert(EL_LightComponent light)
	{
		foreach(EL_BaseEntry entry : m_Entries)
		{
			entry.OnRegister(light);
		}
	}
	
	void SetDelay(float delay)
	{
		m_Timer += delay;
	}
	
	void GoBack(int steps)
	{
		int nextIdx = m_EntryIndex - steps;
		if(nextIdx < 0) m_EntryIndex = 0;
		else m_EntryIndex = nextIdx;
	}
	
	void Reset()
	{
		m_EntryIndex = 0;
	}
	
	protected void Simulate(float timeSlice)
	{
		m_Timer -= timeSlice;
		while(m_Timer <= 0 && m_EntryIndex < m_Entries.Count()) 
		{
			m_Entries[m_EntryIndex].OnExecute(this);
			m_EntryIndex++;
		}
	}
	
	void Tick(float timeSlice)
	{
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
	
	void SetPrecision(int precision)
	{
		if(precision < 1) m_Precision = 1;
		else m_Precision = precision;
	}
	
}
