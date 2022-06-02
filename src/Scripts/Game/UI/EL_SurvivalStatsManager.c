class EL_SurvivalStatsManagerComponentClass: GameComponentClass
{
};

class EL_SurvivalStatsManagerComponent : GameComponent
{
	[Attribute()]
	protected float DecaySpeed;
	
	float clientHunger = 100.0;
	float clientThirst = 100.0;
	
	float currentHunger = 0.0;
	float currentThirst = 0.0;
	
	//on player spawn / start begin decaying client hunger and thirst by 1 and 1.2 respectively at an interval of DecaySpeed
	//set two functions to get and send back the current values of client hunger and clientThirst
	
	//add a function that if a value is at 0 then damage should occur, if both are 0 then take more damage.
	
	void DriverFunc()
	{
		float counter = DecaySpeed;
		currentThirst = clientThirst;
		currentHunger = clientHunger;
		
		while (IsActive())
		{
			if (counter <= 0.0)
			{
				currentThirst = currentThirst - 1.2;
				currentHunger = currentThirst - 1.0;
				counter = DecaySpeed;
			}
			else
			{
				counter - 0.1;
				continue;
			}
		}
	}
	
	float GetThirst()
	{
		return currentThirst;
	}
	
	float GetHunger()
	{
		return currentHunger;
	}
	
}