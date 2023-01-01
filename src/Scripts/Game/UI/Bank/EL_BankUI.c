modded enum ChimeraMenuPreset
{
	EL_BankMenu,
	EL_BankDepositDialog,
	EL_BankWithdrawDialog,
	EL_BankTransferDialog
}

class EL_BankDepositDialog : EL_BankDialogBase
{
	//------------------------------------------------------------------------------------------------
	override protected void OnConfirm()
	{
		if (!m_wMoneyEditBox.GetText().IsEmpty())
			m_BankManager.Deposit(m_BankManager.GetLocalPlayerBankAccount(), m_wMoneyEditBox.GetText().ToInt());
		super.OnConfirm();
	}
}

class EL_BankWithdrawDialog : EL_BankDialogBase
{
	//------------------------------------------------------------------------------------------------
	override protected void OnConfirm()
	{
		if (!m_wMoneyEditBox.GetText().IsEmpty())
			m_BankManager.TryWithdraw(m_BankManager.GetLocalPlayerBankAccount(), m_wMoneyEditBox.GetText().ToInt());
		super.OnConfirm();
	}
}

class EL_BankTransferDialog : EL_BankDialogBase
{
}

class EL_BankDialogBase : DialogUI
{
	protected EL_GlobalBankAccountManager m_BankManager;
	protected EditBoxWidget m_wMoneyEditBox;

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();

		m_wMoneyEditBox = EditBoxWidget.Cast(GetRootWidget().FindAnyWidget("MoneyAmount"));
		m_BankManager = EL_GlobalBankAccountManager.GetInstance();
	}
}


class EL_BankMenu : ChimeraMenuBase
{
    protected Widget m_wRoot;
	protected TextWidget m_wCurrentCash, m_wCurrentBalance;
	protected EL_GlobalBankAccountManager m_BankManager;
	
	//------------------------------------------------------------------------------------------------
	void OpenDepositMenu()
	{
		EL_BankDepositDialog.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.EL_BankDepositDialog));
	}

	//------------------------------------------------------------------------------------------------
	void OpenWithdrawMenu()
	{
		EL_BankWithdrawDialog withdrawDialog = EL_BankWithdrawDialog.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.EL_BankWithdrawDialog));
		withdrawDialog.SetConfirmText("Withdraw");
	}

	//------------------------------------------------------------------------------------------------
	void OpenTransferMenu()
	{
		EL_BankTransferDialog transferDialog = EL_BankTransferDialog.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.EL_BankTransferDialog));
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateCashText()
	{
		m_wCurrentCash.SetText(EL_FormatUtils.DecimalSeperator(EL_MoneyUtils.GetCash(SCR_PlayerController.GetLocalControlledEntity())));
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateBalanceText()
	{
		m_wCurrentBalance.SetText(EL_FormatUtils.DecimalSeperator(m_BankManager.GetBalance(m_BankManager.GetLocalPlayerBankAccount())));
	}
			
    //------------------------------------------------------------------------------------------------
    override void OnMenuOpen()
    {
        m_wRoot = GetRootWidget();

		m_wCurrentCash = TextWidget.Cast(m_wRoot.FindAnyWidget("CurrentMoney"));
		m_wCurrentBalance = TextWidget.Cast(m_wRoot.FindAnyWidget("CurrentBalance"));
		
		SCR_ButtonComponent depositButtonHandler = SCR_ButtonComponent.Cast(m_wRoot.FindAnyWidget("DepositButton").FindHandler(SCR_ButtonComponent));
		SCR_ButtonComponent withdrawButtonHandler = SCR_ButtonComponent.Cast(m_wRoot.FindAnyWidget("WithdrawButton").FindHandler(SCR_ButtonComponent));
		SCR_ButtonComponent transferButtonHandler = SCR_ButtonComponent.Cast(m_wRoot.FindAnyWidget("TransferButton").FindHandler(SCR_ButtonComponent));
		SCR_ButtonComponent exitButtonHandler = SCR_ButtonComponent.Cast(m_wRoot.FindAnyWidget("ExitButton").FindHandler(SCR_ButtonComponent));

		depositButtonHandler.m_OnClicked.Insert(OpenDepositMenu);
		withdrawButtonHandler.m_OnClicked.Insert(OpenWithdrawMenu);
		transferButtonHandler.m_OnClicked.Insert(OpenTransferMenu);
		exitButtonHandler.m_OnClicked.Insert(Close);
		
		m_BankManager = EL_GlobalBankAccountManager.GetInstance();

    }

	//------------------------------------------------------------------------------------------------
	override void OnMenuFocusGained()
	{
		UpdateCashText();
		UpdateBalanceText();
	}
}