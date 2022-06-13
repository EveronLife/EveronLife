class EL_BetaHud : SCR_InfoDisplay
{
	private SliderWidget m_StaminaSlider;
	private SliderWidget m_HealthSlider;
	private SliderWidget m_ThirstSlider;
	private SliderWidget m_HungerSlider;
	private TextWidget m_MoneyDisplay;
	private FrameWidget m_PlayerStatsFrame;
	private FrameWidget m_PlayerStatsIcons;
	private SCR_CharacterControllerComponent m_PlayerController;
	private DamageManagerComponent m_DMC;
	
	private bool m_StatChange;
	private float m_PreviousValues[4];
	//TODO: Money & Survival Stats
	
	//--------------------------- OnChangeFunctions --------------------------- 
	void OnHealthChange(float value)
	{
		if (!m_HealthSlider)
		{
			m_HealthSlider = SliderWidget.Cast(m_PlayerStatsFrame.FindWidget("healthSlider"));
			if (!m_HealthSlider) return;
		}
		
		private float m_currentHealth = m_DMC.GetHealth();
		
		if (!m_PreviousValues[0])
		{
			m_PreviousValues[0] = m_currentHealth;
		}
		else
		{
			if (float.AlmostEqual(m_PreviousValues[0], m_currentHealth)) return;
			
			m_HealthSlider.SetCurrent(value);
			m_StatChange = true;
		}
	}
	
	void OnStaminaChange(float value)
	{
		if (!m_StaminaSlider)
		{
			m_StaminaSlider = SliderWidget.Cast(m_PlayerStatsFrame.FindWidget("staminaSlider"));
			if (!m_StaminaSlider) return;
		}
		
		private float m_currentStam = m_PlayerController.GetStamina();
		
		if (!m_PreviousValues[1])
		{
			m_PreviousValues[1] = m_currentStam;
		}
		else
		{
			if (!float.AlmostEqual(m_PreviousValues[1] , m_currentStam))
			{
				m_PreviousValues[1] = m_currentStam;
				m_StaminaSlider.SetCurrent(value);
				m_StatChange = true;
			}
			else
			{
				return;
			}
		}
	}
	
	void OnThirstChange(float value)
	{
		if (!m_ThirstSlider)
		{
			m_ThirstSlider = SliderWidget.Cast(m_PlayerStatsFrame.FindWidget("thirstSlider"));
			if (!m_ThirstSlider) return;
		}
		
		m_ThirstSlider.SetCurrent(value);
		//m_StatChange = true;
	}
	
	void OnHungerChange(float value)
	{
		if (!m_HungerSlider)
		{
			m_HungerSlider = SliderWidget.Cast(m_PlayerStatsFrame.FindWidget("hungerSlider"));
			if (!m_HungerSlider) return;
		}
		
		m_HungerSlider.SetCurrent(value);
		//m_StatChange = true;
	}
	
	void OnMoneyChange(float value)
	{
		if (m_MoneyDisplay)
		{
			m_MoneyDisplay = TextWidget.Cast(m_PlayerStatsFrame.FindWidget("moneyDisplay"));
			if (!m_MoneyDisplay) return;
		}
		
		m_MoneyDisplay.SetText("$ " + value); //for configurabiluity could have the $ changeable in config...
		//m_StatChange = true;
	}
	
	//--------------------------- Main Functions --------------------------- 
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);

		IEntity player = GetGame().GetPlayerController();
		if (!player) return;
		
		m_PlayerController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
		m_PlayerStatsFrame = FrameWidget.Cast(m_wRoot.FindWidget("playerStatsFrame"));
		if (!m_PlayerStatsFrame) return;
		
		m_PlayerStatsIcons = FrameWidget.Cast(m_wRoot.FindWidget("playerStatsIcons"));
		if(!m_PlayerStatsIcons) return;
	}
	
	
	//UpdateValues needs to be called upon respawning as to reset the UI for m_HealthSlider so it isnt stuck at 0 until you take damage -- KNOWN BUG
	float m_TimeAccumulator = 0;
	bool m_GUIHidden = false;
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
			
			//TODO: Get Money Stats Component
		}
		
		m_StatChange = false;
		OnHealthChange(m_DMC.GetHealth());
		OnStaminaChange(m_PlayerController.GetStamina());
		//TODO: Get info from Money and Survival Stats Components
		

		if (m_StatChange)
		{
			m_TimeAccumulator = 0;
			if(m_GUIHidden)
			{
				m_GUIHidden = false;
				ShowStatsHUD(true);
			}
		}
		else
		{
			m_TimeAccumulator += timeSlice;
			if (m_TimeAccumulator > 4.0 && !m_GUIHidden)
			{
				m_GUIHidden = true;
				ShowStatsHUD(false);
			}
		}
	}
	
	void ShowStatsHUD(bool var) //TODO: Key press to also show stats HUD? " ` or ~ key perhaps, KeyCode.KC_GRAVE"
	{
		if (var)
		{
			WidgetAnimator.PlayAnimation(m_PlayerStatsIcons,WidgetAnimationType.Opacity,true,WidgetAnimator.FADE_RATE_DEFAULT);
			WidgetAnimator.PlayAnimation(m_PlayerStatsFrame,WidgetAnimationType.Opacity,true,WidgetAnimator.FADE_RATE_SLOW);
		}
		else
		{
			WidgetAnimator.PlayAnimation(m_PlayerStatsFrame,WidgetAnimationType.Opacity,false,WidgetAnimator.FADE_RATE_DEFAULT);
			WidgetAnimator.PlayAnimation(m_PlayerStatsIcons,WidgetAnimationType.Opacity,false,3);
		}
	}
}