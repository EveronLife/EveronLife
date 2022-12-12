class EL_SplitQuantityDialog : DialogUI
{
	protected EL_QuantityComponent m_pQuantitySource;
	protected EditBoxWidget m_wKeepAmount;
	protected EditBoxWidget m_wSplitAmount;
	protected EL_ThinSliderWidget m_wSlider;

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		Widget w = GetRootWidget();
		m_wKeepAmount = EditBoxWidget.Cast(w.FindAnyWidget("KeepAmount"));
		m_wSplitAmount = EditBoxWidget.Cast(w.FindAnyWidget("SplitAmount"));
		m_wSlider = EL_ThinSliderWidget.Cast(w.FindAnyWidget("AmountSlider").FindHandler(EL_ThinSliderWidget));
		m_wSlider.GetOnValueChanged().Insert(OnSliderValueChanged);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnConfirm()
	{
		if (m_pQuantitySource) m_OnConfirm.Invoke(m_pQuantitySource, (int)(m_wSlider.GetCurrent() + 1));
		CloseAnimated();
	}

	//------------------------------------------------------------------------------------------------
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if ((w == m_wKeepAmount || w == m_wSplitAmount) && m_wKeepAmount.GetText() && m_wSplitAmount.GetText())
		{
			int quantity = m_pQuantitySource.GetQuantity();
			int keepAmount, splitAmount;

			if (w == m_wKeepAmount)
			{
				keepAmount = EL_Utils.MaxInt(EL_Utils.MinInt(m_wKeepAmount.GetText().ToInt(), quantity - 1), 1);
				splitAmount = quantity - keepAmount;
			}
			else
			{
				splitAmount = EL_Utils.MaxInt(EL_Utils.MinInt(m_wSplitAmount.GetText().ToInt(), quantity - 1), 1);
				keepAmount = quantity - splitAmount;
			}

			m_wKeepAmount.SetText(keepAmount.ToString());
			m_wSplitAmount.SetText(splitAmount.ToString());
			m_wSlider.SetCurrent(splitAmount - 1);
		}

		return super.OnChange(w, x, y, finished);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnSliderValueChanged(float value, bool final)
	{
		int splitSize = value + 1;
		m_wKeepAmount.SetText(string.Format("%1", m_pQuantitySource.GetQuantity() - splitSize));
		m_wSplitAmount.SetText(string.Format("%1", splitSize));
	}

	//------------------------------------------------------------------------------------------------
	void SetQuantitySource(notnull EL_QuantityComponent quantitySource)
	{
		m_pQuantitySource = quantitySource;
		int quantity = quantitySource.GetQuantity();
		int half = quantity / 2;
		m_wSlider.Configure(0, quantity - 2, 1, half - 1);
		m_wKeepAmount.SetText(string.Format("%1", quantity - half));
		m_wSplitAmount.SetText(string.Format("%1", half));
	}

	//------------------------------------------------------------------------------------------------
	static EL_SplitQuantityDialog CreateSplitQuantityDialog(notnull EL_QuantityComponent quantitySource)
	{
		EL_SplitQuantityDialog splitDialog = EL_SplitQuantityDialog.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.EL_SplitQuantityDialog));
		if (!splitDialog) return null;
		splitDialog.SetQuantitySource(quantitySource);
		return splitDialog;
	}
}
