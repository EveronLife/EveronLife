class EL_BetaHud : SCR_InfoDisplay
{
	private ProgressBarWidget m_StaminaProgressBar;
	private ImageWidget m_StaminaProgress;
	private ImageWidget m_HealthProgress;
	private ImageWidget m_ThirstProgress;
	private ImageWidget m_HungerProgress;
	private TextWidget m_MoneyIndicator;
	private HorizontalLayoutWidget m_PlayerStatsHUD;
	private OverlayWidget m_HealthIndicator;
	private OverlayWidget m_StaminaIndicator;
	private OverlayWidget m_HungerIndicator;
	private OverlayWidget m_ThirstIndicator;
	private SCR_CharacterControllerComponent m_PlayerController;
	private DamageManagerComponent m_DMC;
	
	//--------------------------- CONFIG VALUES ---------------------------
	private bool m_EnableHUD = true; //TODO: ref from settings insted of setting in code
	private bool m_EnableStamina = true;
	private bool m_EnableHealth = true;
	private bool m_EnableThirst = true;
	private bool m_EnableHunger = true;
	private bool m_EnableMoney = true;
	//---------------------------------------------------------------------
	
	private bool m_UsingProgressWidget;
	private bool m_StatChange;
	private float m_PreviousValues[4];
	//TODO: Money & Survival Stats
	
	//--------------------------- OnChangeFunctions --------------------------- 
	void OnHealthChange(float value)
	{
		if (!m_HealthProgress)
		{
			m_HealthProgress = ImageWidget.Cast(m_HealthIndicator.FindAnyWidget("m_healthProgress"));
			if (!m_HealthProgress) return;
		}
		
		private float m_currentHealth = m_DMC.GetHealth() / 100;
		
		if (!m_PreviousValues[0])
		{
			m_PreviousValues[0] = m_currentHealth;
		}
		else
		{
			if (float.AlmostEqual(m_PreviousValues[0], m_currentHealth)) return;
			
			m_HealthProgress.SetMaskProgress(value);
			m_StatChange = true;
			
			//TODO: Make this only call every once and a while similar to the fade
			SetProgressColor(m_HealthProgress, value);
		}
	}
	
	void OnStaminaChange(float value)
	{
		if (!m_StaminaProgressBar)
		{
			if (!m_StaminaProgress)
			{
				m_StaminaProgress = ImageWidget.Cast(m_StaminaIndicator.FindAnyWidget("m_staminaProgress"));
				if (!m_StaminaProgress) return;
			}
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
				if (!m_StaminaProgressBar)
				{
					m_StaminaProgress.SetMaskProgress(value);
					SetProgressColor(m_StaminaProgress, value);//TODO: Make this only call every once and a while similar to the fade
				}
				else
				{
					m_StaminaProgressBar.SetCurrent(value);
					SetProgressColor(m_StaminaProgressBar, value);//TODO: Make this only call every once and a while similar to the fade
				}
				
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
		if (!m_ThirstProgress)
		{
			m_ThirstProgress = ImageWidget.Cast(m_ThirstIndicator.FindAnyWidget("m_thirstProgress"));
			if (!m_ThirstProgress) return;
		}
		
		m_ThirstProgress.SetMaskProgress(value);
		//m_StatChange = true;
	}
	
	void OnHungerChange(float value)
	{
		if (!m_HungerProgress)
		{
			m_HungerProgress = ImageWidget.Cast(m_HungerIndicator.FindAnyWidget("m_hungerProgress"));
			if (!m_HungerProgress) return;
		}
		
		m_HungerProgress.SetMaskProgress(value);
		//m_StatChange = true;
	}
	
	void OnMoneyChange(float value)
	{
		if (m_MoneyIndicator)
		{
			m_MoneyIndicator = TextWidget.Cast(m_PlayerStatsHUD.FindAnyWidget("m_moneyIndicator"));
			if (!m_MoneyIndicator) return;
		}
		
		m_MoneyIndicator.SetText("$ " + value); //for configurabiluity could have the $ changeable in config...
		//m_StatChange = true;
	}
	
	//--------------------------- Main Functions --------------------------- 
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		
		if (!m_EnableHUD) return;
		
		IEntity player = GetGame().GetPlayerController();
		if (!player) return;
		
		m_PlayerController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
		
		m_PlayerStatsHUD = HorizontalLayoutWidget.Cast(m_wRoot.FindAnyWidget("m_playerStatsHUD"));
		if (!m_PlayerStatsHUD) return;
		
		if (m_EnableHealth)
		{
			m_HealthIndicator = OverlayWidget.Cast(m_PlayerStatsHUD.FindAnyWidget("m_healthIndicator"));
			if (!m_HealthIndicator) return;
		}
		
		if (m_EnableStamina)
		{
			m_StaminaIndicator = OverlayWidget.Cast(m_PlayerStatsHUD.FindAnyWidget("m_staminaIndicator"));
			//if this is null thats fine as long as the ProgressBarWidget is enabled
			
			if (!m_StaminaIndicator || !m_StaminaIndicator.IsEnabled())
			{
				m_UsingProgressWidget = true;
				m_StaminaProgressBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_staminaProgressBar"));
				if (!m_StaminaProgressBar) return;
			}
			else
			{
				m_UsingProgressWidget = false;
			}
		}
		
		if (m_EnableHunger)
		{
			m_HungerIndicator = OverlayWidget.Cast(m_PlayerStatsHUD.FindAnyWidget("m_hungerIndicator"));
			if (!m_HungerIndicator) return;
		}
		
		if (m_EnableThirst)
		{
			m_ThirstIndicator = OverlayWidget.Cast(m_PlayerStatsHUD.FindAnyWidget("m_thirstIndicator"));
			if (!m_ThirstIndicator) return;
		}
	}
	
	
	//TODO: UpdateValues needs to be called upon respawning as to reset the UI for m_HealthProgress so it isnt stuck at 0 until you take damage
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
		if (m_EnableHealth) OnHealthChange(m_DMC.GetHealth());
		if (m_EnableStamina) OnStaminaChange(m_PlayerController.GetStamina());
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
	
	void ShowStatsHUD(bool var) //TODO: Show HUD when in an inventory
	{
		if (var)
		{
			WidgetAnimator.PlayAnimation(m_PlayerStatsHUD,WidgetAnimationType.Opacity,true,WidgetAnimator.FADE_RATE_SLOW);
			if (m_UsingProgressWidget)
			{
				WidgetAnimator.PlayAnimation(m_StaminaProgress,WidgetAnimationType.Opacity,true,WidgetAnimator.FADE_RATE_SLOW);
			}
		}
		else
		{
			WidgetAnimator.PlayAnimation(m_PlayerStatsHUD,WidgetAnimationType.Opacity,false,WidgetAnimator.FADE_RATE_SLOW);
			if (m_UsingProgressWidget)
			{
				WidgetAnimator.PlayAnimation(m_StaminaProgress,WidgetAnimationType.Opacity,false,WidgetAnimator.FADE_RATE_SLOW);
			}
		}
	}
	
	//TODO: Make this only call every once and a while similar to the fade
	void SetProgressColor(Widget bar, float value)
	{
		if (value >= 0.65)
		{
			//turn white
			bar.SetColor(Color.White);
		}
		else if (value > 0.35 && value < 0.65)
		{
			//turn yellow
			bar.SetColor(Color.Yellow);
		}
		else
		{
			//turn red
			bar.SetColor(Color.Red);
		}
	}
}