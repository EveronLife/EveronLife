class EL_BetaHud : SCR_InfoDisplay
{
	private SliderWidget m_StaminaSlider;
	private SliderWidget m_HealthSlider;
	private SliderWidget m_ThirstSlider;
	private SliderWidget m_HungerSlider;
	private TextWidget m_MoneyDisplay;
	private SCR_CharacterControllerComponent m_PlayerController;
	private DamageManagerComponent m_DMC;
	//TODO: Money & Survival Stats
	
	//--------------------------- OnChangeFunctions --------------------------- 
	void OnHealthChange(float value)
	{
		if (!m_HealthSlider)
		{
			m_HealthSlider = SliderWidget.Cast(m_wRoot.FindWidget("healthSlider"));
			if (!m_HealthSlider) return;
		};
		
		m_HealthSlider.SetCurrent(value)
	}
	
	void OnStaminaChange(float value)
	{
		if (!m_StaminaSlider)
		{
			m_StaminaSlider = SliderWidget.Cast(m_wRoot.FindWidget("staminaSlider"));
			if (!m_StaminaSlider) return;
		};
		
		m_StaminaSlider.SetCurrent(value);
	}
	
	void OnThirstChange(float value)
	{
		if (!m_ThirstSlider)
		{
			m_ThirstSlider = SliderWidget.Cast(m_wRoot.FindWidget("thirstSlider"));
			if (!m_ThirstSlider) return;
		};
		
		m_ThirstSlider.SetCurrent(value);
	}
	
	void OnHungerChange(float value)
	{
		if (!m_HungerSlider)
		{
			m_HungerSlider = SliderWidget.Cast(m_wRoot.FindWidget("hungerSlider"));
			if (!m_HungerSlider) return;
		};
		
		m_HungerSlider.SetCurrent(value);
	}
	
	void OnMoneyChange(float value)
	{
		if (m_MoneyDisplay)
		{
			m_MoneyDisplay = TextWidget.Cast(m_wRoot.FindWidget("moneyDisplay"));
			if (!m_MoneyDisplay) return;
		};
		
		m_MoneyDisplay.SetText("$ " + value) //for configurabiluity could have the $ changeable in config...
	}
	
	//--------------------------- Main Functions --------------------------- 
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);

		IEntity player = GetGame().GetPlayerController();
		if (!player) return;
		
		m_PlayerController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
	}
	
	
	//UpdateValues needs to be called upon respawning as to reset the UI for m_HealthSlider so it isnt stuck at 0 until you take damage -- KNOWN BUG
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		if (!m_PlayerController)
		{
			IEntity player = SCR_PlayerController.GetLocalControlledEntity();
			if (!player) return;
		
			m_PlayerController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
			if (!m_PlayerController) return;
			
			m_DMC = DamageManagerComponent.Cast(player.FindComponent(DamageManagerComponent));
			if (!m_DMC) return;
			
			//TODO: Get Survival Stats Component
			
			//TOFO: Get Money Stats Component
		}; 
		
		OnHealthChange(m_DMC.GetHealth());
		OnStaminaChange(m_PlayerController.GetStamina());
		//TODO: Get info from Money and Survival Stats Components
	}
}