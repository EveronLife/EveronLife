class EL_BetaHud : SCR_InfoDisplay
{
	private SliderWidget staminaSlider = null;
	private SliderWidget healthSlider = null;
	private SliderWidget thirstSlider = null;
	private SliderWidget hungerSlider = null;
	private TextWidget moneyDisplay = null;
	private SCR_CharacterControllerComponent playerController = null;
	private DamageManagerComponent DMC = null;
	
	//--------------------------- OnChangeFunctions --------------------------- 
	void OnHealthChange(float value)
	{
		if (!healthSlider)
		{
			healthSlider = SliderWidget.Cast(m_wRoot.FindWidget("healthSlider"));
			if (!healthSlider) return;
		};
		
		healthSlider.SetCurrent(value)
	}
	
	void OnStaminaChange(float value)
	{
		if (!staminaSlider)
		{
			staminaSlider = SliderWidget.Cast(m_wRoot.FindWidget("staminaSlider"));
			if (!staminaSlider) return;
		};
		
		staminaSlider.SetCurrent(value);
	}
	
	void OnThirstChange(float value)
	{
		if (!thirstSlider)
		{
			thirstSlider = SliderWidget.Cast(m_wRoot.FindWidget("thirstSlider"));
			if (!thirstSlider) return;
		};
		
		thirstSlider.SetCurrent(value);
	}
	
	void OnHungertChange(float value)
	{
		if (!hungerSlider)
		{
			hungerSlider = SliderWidget.Cast(m_wRoot.FindWidget("hungerSlider"));
			if (!hungerSlider) return;
		};
		
		hungerSlider.SetCurrent(value);
	}
	
		void OnMoneyChange(float value)
	{
		if (moneyDisplay)
		{
			moneyDisplay = TextWidget.Cast(m_wRoot.FindWidget("moneyDisplay"));
			if (!moneyDisplay) return;
		};
		
		moneyDisplay.SetText("$ " + value) //for configurabiluity could have the $ changeable in config...
	}
	
	//--------------------------- Main Functions --------------------------- 
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);

		staminaSlider = SliderWidget.Cast(m_wRoot.FindWidget("staminaSlider"));
		healthSlider = SliderWidget.Cast(m_wRoot.FindWidget("healthSlider"));
		IEntity player = GetGame().GetPlayerController();
		if (!player) {
			Print("no player found");
			return;
		}
		
		playerController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
		if (playerController) Print("Found character controller component")
	}
	
	
	//UpdateValues needs to be called upon respawning as to reset the UI for healthSlider so it isnt stuck at 0 until you take damage -- KNOWN BUG
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		if (!playerController)
		{
			IEntity player = SCR_PlayerController.GetLocalControlledEntity();
			if (!player) {
				return;
			}
		
			playerController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
			if (!playerController) return;
			
			DMC = DamageManagerComponent.Cast(player.FindComponent(DamageManagerComponent));
			if (!DMC) return;
			
			//add lines to get thirst, hunger, money here
			
		};
		
		OnStaminaChange(playerController.GetStamina());
		OnHealthChange(DMC.GetHealth())
	}
}