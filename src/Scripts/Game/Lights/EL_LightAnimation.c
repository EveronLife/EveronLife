[BaseContainerProps()]
class EL_LightAnimation
{
	[Attribute()]
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
	
	protected void Simulate(float timeSlice)
	{
		m_Timer -= timeSlice;
		while(m_Timer <= 0) 
		{
			m_Entries[m_EntryIndex].OnExecute(this);
			m_EntryIndex = (m_EntryIndex + 1) % (m_Entries.Count());
		}
	}
	
	void Tick(float timeSlice)
	{
		for(int i = 0; i < m_Precision; i++)
		{
			Simulate(timeSlice / m_Precision);
		}
	}
	
	void SetPrecision(int precision)
	{
		if(precision < 1) m_Precision = 1;
		else m_Precision = precision;
	}
	
}
