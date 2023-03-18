class EL_BetaHud : SCR_InfoDisplay
{
	//--------------------------- CONFIG VALUES ---------------------------
	//! TODO: ref from settings insted of setting in code
	protected bool m_bEnableHUD = true;
	protected bool m_bEnableStamina = true;
	protected bool m_bEnableHealth = true;
	protected bool m_bEnableThirst = true;
	protected bool m_bEnableHunger = true;
	protected bool m_bEnableMoney = true;
	//---------------------------------------------------------------------

	protected HorizontalLayoutWidget m_wPlayerStatsHUD;

	protected ProgressBarWidget m_wStaminaProgressBar;
	protected ImageWidget m_wStaminaProgress;
	protected ImageWidget m_wHealthProgress;
	protected ImageWidget m_wThirstProgress;
	protected ImageWidget m_wHungerProgress;

	protected TextWidget m_wMoneyIndicator;
	protected OverlayWidget m_wHealthIndicator;
	protected OverlayWidget m_wStaminaIndicator;
	protected OverlayWidget m_wHungerIndicator;
	protected OverlayWidget m_wThirstIndicator;

	protected SCR_CharacterControllerComponent m_PlayerController;
	protected DamageManagerComponent m_DMC;

	protected bool m_bUsingProgressWidget;
	protected bool m_bStatChange;
	protected float m_fPreviousValues[4];

	protected float m_fTimeAccumulator;
	protected bool m_bGUIHidden;

	//---------------------------------------------------------------------
	void OnHealthChange(float value)
	{
		if (!m_wHealthProgress)
		{
			m_wHealthProgress = ImageWidget.Cast(m_wHealthIndicator.FindAnyWidget("m_healthProgress"));
			if (!m_wHealthProgress) return;
		}

		float currentHealth = m_DMC.GetHealth() / 100;

		if (!m_fPreviousValues[0])
		{
			m_fPreviousValues[0] = currentHealth;
		}
		else
		{
			if (float.AlmostEqual(m_fPreviousValues[0], currentHealth)) return;

			m_wHealthProgress.SetMaskProgress(value);
			m_bStatChange = true;

			//TODO: Make this only call every once and a while similar to the fade
			SetProgressColor(m_wHealthProgress, value);
		}
	}

	//---------------------------------------------------------------------
	void OnStaminaChange(float value)
	{
		if (!m_wStaminaProgressBar)
		{
			if (!m_wStaminaProgress)
			{
				m_wStaminaProgress = ImageWidget.Cast(m_wStaminaIndicator.FindAnyWidget("m_staminaProgress"));
				if (!m_wStaminaProgress) return;
			}
		}

		float m_currentStam = m_PlayerController.GetStamina();

		if (!m_fPreviousValues[1])
		{
			m_fPreviousValues[1] = m_currentStam;
		}
		else
		{
			if (!float.AlmostEqual(m_fPreviousValues[1] , m_currentStam))
			{
				m_fPreviousValues[1] = m_currentStam;
				if (!m_wStaminaProgressBar)
				{
					m_wStaminaProgress.SetMaskProgress(value);
					SetProgressColor(m_wStaminaProgress, value);//TODO: Make this only call every once and a while similar to the fade
				}
				else
				{
					m_wStaminaProgressBar.SetCurrent(value);
					SetProgressColor(m_wStaminaProgressBar, value);//TODO: Make this only call every once and a while similar to the fade
				}

				m_bStatChange = true;

			}
		}
	}

	//---------------------------------------------------------------------
	void OnThirstChange(float value)
	{
		if (!m_wThirstProgress)
		{
			m_wThirstProgress = ImageWidget.Cast(m_wThirstIndicator.FindAnyWidget("m_thirstProgress"));
			if (!m_wThirstProgress) return;
		}

		m_wThirstProgress.SetMaskProgress(value);
		//m_bStatChange = true;
	}

	//---------------------------------------------------------------------
	void OnHungerChange(float value)
	{
		if (!m_wHungerProgress)
		{
			m_wHungerProgress = ImageWidget.Cast(m_wHungerIndicator.FindAnyWidget("m_hungerProgress"));
			if (!m_wHungerProgress) return;
		}

		m_wHungerProgress.SetMaskProgress(value);
		//m_bStatChange = true;
	}

	//---------------------------------------------------------------------
	void OnMoneyChange(ResourceName prefab, int newAmount, int oldAmount)
	{
		if (!m_wMoneyIndicator)
		{
			m_wMoneyIndicator = TextWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_moneyIndicator"));
			if (!m_wMoneyIndicator) return;
		}

		m_wMoneyIndicator.SetText("$ " + EL_FormatUtils.AbbreviateNumber(newAmount)); //for configurabiluity could have the $ changeable in config...
		//m_bStatChange = true;
	}

	//---------------------------------------------------------------------
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);

		if (!m_bEnableHUD) return;

		m_PlayerController = SCR_CharacterControllerComponent.Cast(owner.FindComponent(SCR_CharacterControllerComponent));
		m_DMC = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		m_wPlayerStatsHUD = HorizontalLayoutWidget.Cast(m_wRoot.FindAnyWidget("m_playerStatsHUD"));

		if (!m_wPlayerStatsHUD) return;

		if (m_bEnableHealth)
		{
			m_wHealthIndicator = OverlayWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_healthIndicator"));
			if (!m_wHealthIndicator) return;
		}

		if (m_bEnableStamina)
		{
			m_wStaminaIndicator = OverlayWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_staminaIndicator"));
			//If this is null thats fine as long as the ProgressBarWidget is enabled

			m_bUsingProgressWidget = (!m_wStaminaIndicator || !m_wStaminaIndicator.IsEnabled());
			if (m_bUsingProgressWidget)
			{
				m_wStaminaProgressBar = ProgressBarWidget.Cast(m_wRoot.FindAnyWidget("m_staminaProgressBar"));
				if (!m_wStaminaProgressBar) return;
			}
		}

		if (m_bEnableHunger)
		{
			m_wHungerIndicator = OverlayWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_hungerIndicator"));
			if (!m_wHungerIndicator) return;
		}

		if (m_bEnableThirst)
		{
			m_wThirstIndicator = OverlayWidget.Cast(m_wPlayerStatsHUD.FindAnyWidget("m_thirstIndicator"));
			if (!m_wThirstIndicator) return;
		}

		// Init cash display and subscribe to balance changes
		int currentCash = EL_MoneyUtils.GetCash(owner);
		OnMoneyChange(EL_MoneyUtils.PREFAB_CASH, currentCash, currentCash);
		ScriptedInventoryStorageManagerComponent inventoryManager = EL_ComponentFinder<ScriptedInventoryStorageManagerComponent>.Find(owner);
		if (inventoryManager) inventoryManager.EL_GetOnAmountChanged(EL_MoneyUtils.PREFAB_CASH).Insert(OnMoneyChange);
	}

	//---------------------------------------------------------------------
	//! TODO: UpdateValues needs to be called upon respawning as to reset the UI for m_wHealthProgress so it isnt stuck at 0 until you take damage
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		if (!m_PlayerController) return;

		m_bStatChange = false;
		if (m_bEnableHealth) OnHealthChange(m_DMC.GetHealth());
		if (m_bEnableStamina) OnStaminaChange(m_PlayerController.GetStamina());
		//TODO: Get info from Survival Stats Components

		if (m_bStatChange)
		{
			m_fTimeAccumulator = 0;
			if(m_bGUIHidden)
			{
				m_bGUIHidden = false;
				ShowStatsHUD(true);
			}
		}
		else
		{
			m_fTimeAccumulator += timeSlice;
			if (m_fTimeAccumulator > 4.0 && !m_bGUIHidden)
			{
				m_bGUIHidden = true;
				ShowStatsHUD(false);
			}
		}
	}

	//---------------------------------------------------------------------
	void ShowStatsHUD(bool show) //TODO: Show HUD when in an inventory
	{
		AnimateWidget.Opacity(m_wPlayerStatsHUD, show, UIConstants.FADE_RATE_SLOW);

		if (m_bUsingProgressWidget)
		{
			AnimateWidget.Opacity(m_wStaminaProgress, show, UIConstants.FADE_RATE_SLOW);
		}
	}

	//---------------------------------------------------------------------
	//! TODO: Make this only call every once and a while similar to the fade
	void SetProgressColor(Widget bar, float value)
	{
		if (value >= 0.65)
		{
			bar.SetColor(Color.White);
		}
		else if (value > 0.35 && value < 0.65)
		{
			bar.SetColor(Color.Yellow);
		}
		else
		{
			bar.SetColor(Color.Red);
		}
	}
}
