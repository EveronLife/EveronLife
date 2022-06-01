class EL_SurvivalStats
{
	private float DecaySpeed = 10.0; //time in seconds, current times are for testing
	private float counter = 0.0;
	
	private float thirstStat = 100.0;
	private float hungerStat = 100.0;
	
	float GetHunger()
	{
		return hungerStat;
	}
	
	float GetThirst()
	{
		return thirstStat;
	}
	
	void SurvivalStats()
	{
		counter = DecaySpeed;
		
		while (counter > 0.0)
		{
			counter - 1.0;
		};
		
		thirstStat = thirstStat - 1.0;
		hungerStat = hungerStat - 1.0;
		
		SurvivalStats()
	}
}

// need to store values on Component in the player