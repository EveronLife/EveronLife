class EL_ThinSliderWidget : ScriptedWidgetComponent
{
	[Attribute("SOUND_FE_ITEM_CHANGE")]
	protected string m_sChangeSound;

	protected SliderWidget m_wSlider;
	protected ImageWidget m_wSliderPosIndicator;
	protected ref ScriptInvoker m_pOnValueChanged;

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wSlider = SliderWidget.Cast(w.FindAnyWidget("HiddenSlider"));
		m_wSliderPosIndicator = ImageWidget.Cast(w.FindAnyWidget("SliderPositionIndicator"));
	}

	//------------------------------------------------------------------------------------------------
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_wSlider)
		{
			UpdateSliderIndicator();
			if (m_sChangeSound) SCR_UISoundEntity.SoundEvent(m_sChangeSound);
			if (m_pOnValueChanged) m_pOnValueChanged.Invoke(m_wSlider.GetCurrent(), finished);
		}

		return super.OnChange(w, x, y, finished);
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateSliderIndicator()
	{
		float width, height;
		m_wSlider.GetScreenSize(width, height);
		width = m_wSlider.GetWorkspace().DPIUnscale(width);
		float progress = m_wSlider.GetCurrent() / m_wSlider.GetMax();
		AlignableSlot.SetPadding(m_wSliderPosIndicator, width * progress, 0, 0, 0);
	}

	//------------------------------------------------------------------------------------------------
	ScriptInvoker GetOnValueChanged()
	{
		if (!m_pOnValueChanged) m_pOnValueChanged = new ScriptInvoker();
		return m_pOnValueChanged;
	}

	//------------------------------------------------------------------------------------------------
	void Configure(float min = 0.0, float max = float.MAX, float step = 1.0, float current = 0.0)
	{
		m_wSlider.SetRange(min, max);
		m_wSlider.SetStep(step);
		m_wSlider.SetCurrent(current);

		// Call later because GetScreenSize returns 0 or unscaled values for a while
		GetGame().GetCallqueue().CallLater(UpdateSliderIndicator, 32, false);
	}

	//------------------------------------------------------------------------------------------------
	float GetCurrent()
	{
		return m_wSlider.GetCurrent();
	}

	//------------------------------------------------------------------------------------------------
	void SetCurrent(float curr)
	{
		m_wSlider.SetCurrent(curr);
		UpdateSliderIndicator();
	}
}
