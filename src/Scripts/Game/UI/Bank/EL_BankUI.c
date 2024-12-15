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
			EL_NetworkUtils.GetLocalRpcSender().AskTransactionFromBankAccount(m_wMoneyEditBox.GetText().ToInt(), m_wCommentEditBox.GetText());
		super.OnConfirm();
	}
}

class EL_BankWithdrawDialog : EL_BankDialogBase
{
	//------------------------------------------------------------------------------------------------
	override protected void OnConfirm()
	{
		if (!m_wMoneyEditBox.GetText().IsEmpty())
			EL_NetworkUtils.GetLocalRpcSender().AskTransactionFromBankAccount(-m_wMoneyEditBox.GetText().ToInt(), m_wCommentEditBox.GetText());
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
	protected EditBoxWidget m_wCommentEditBox;

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();

		m_wMoneyEditBox = EditBoxWidget.Cast(GetRootWidget().FindAnyWidget("MoneyAmount"));
		m_wCommentEditBox = EditBoxWidget.Cast(GetRootWidget().FindAnyWidget("CommentText"));
		m_BankManager = EL_GlobalBankAccountManager.GetInstance();
	}
}

class EL_BankMenu : ChimeraMenuBase
{
    protected Widget m_wRoot;
	protected TextWidget m_wCurrentCash, m_wCurrentBalance;
	protected EL_GlobalBankAccountManager m_BankManager;
	protected ResourceName m_BankAccountLayout = "{1C5799C6871DC397}UI/Layouts/Menus/Bank/BankAccountLayout.layout";
	protected ResourceName m_TransactionLayout = "{E1F963FC9CE9C768}UI/Layouts/Menus/Bank/BankTransactionLayout.layout";
	protected Widget m_wActiveAccount;
	protected ref array<ref Widget> m_aAccountWidgets = new array<ref Widget>();

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
		m_wCurrentCash.SetText("$" + EL_FormatUtils.DecimalSeperator(EL_MoneyUtils.GetCash(SCR_PlayerController.GetLocalControlledEntity())));
	}

	//------------------------------------------------------------------------------------------------
	void UpdateBalanceText(Widget accountWidget, EL_BankAccount account)
	{
		TextWidget accountBalanceWidget = TextWidget.Cast(accountWidget.FindAnyWidget("CurrentBalance"));
		accountBalanceWidget.SetText("$" + EL_FormatUtils.DecimalSeperator(account.GetBalance()));
	}

	//------------------------------------------------------------------------------------------------
	void OnAccountButtonToggle(SCR_ModularButtonComponent comp, bool active)
	{
		if (!active)
			return;

		m_wActiveAccount = comp.GetRootWidget();

		SCR_ModularButtonComponent activeAccountHandler = SCR_ModularButtonComponent.Cast(m_wActiveAccount.FindHandler(SCR_ModularButtonComponent));
		EL_BankAccount account = EL_BankAccount.Cast(activeAccountHandler.GetData());

		//Deativate all other widgets
		foreach (Widget accountButton : m_aAccountWidgets)
		{
			SCR_ModularButtonComponent accountWidgetHandler = SCR_ModularButtonComponent.Cast(accountButton.FindHandler(SCR_ModularButtonComponent));
			accountWidgetHandler.SetToggled(false);
		}
	}

	//------------------------------------------------------------------------------------------------
	Widget AddNewAccount(EL_BankAccount account)
	{
		Widget newAccountWidget = Widget.Cast(GetGame().GetWorkspace().CreateWidgets(m_BankAccountLayout, m_wRoot.FindAnyWidget("AccountList")));

		SCR_ModularButtonComponent newAccountWidgetHandler = SCR_ModularButtonComponent.Cast(newAccountWidget.FindHandler(SCR_ModularButtonComponent));
		newAccountWidgetHandler.m_OnToggled.Insert(OnAccountButtonToggle);
		newAccountWidgetHandler.SetData(account);

		TextWidget accountIdWidget = TextWidget.Cast(newAccountWidget.FindAnyWidget("AccoundIDText"));
		accountIdWidget.SetText("Account: " + account.GetId().ToString());

		UpdateBalanceText(newAccountWidget, account);
		m_aAccountWidgets.Insert(newAccountWidget);

		return newAccountWidget;
	}

	//------------------------------------------------------------------------------------------------
	void UpdateTransactions()
	{
		Widget transactionList = m_wRoot.FindAnyWidget("TransactionList");

		//Clear List
		Widget child = transactionList.GetChildren();
		Widget childtemp;
		while (child)
		{
			childtemp = child;
			child = child.GetSibling();
			childtemp.RemoveFromHierarchy();
		}

		//Populate List
		foreach (EL_BankTransaction transaction : m_BankManager.GetLocalPlayerBankAccount().m_aTransactions)
		{
			AddNewTransaction(transaction);
		}
	}

	//------------------------------------------------------------------------------------------------
	void AddNewTransaction(EL_BankTransaction transaction)
	{
		Widget transactionWidget =  Widget.Cast(GetGame().GetWorkspace().CreateWidgets(m_TransactionLayout, m_wRoot.FindAnyWidget("TransactionList")));

		TextWidget dateText = TextWidget.Cast(transactionWidget.FindAnyWidget("Date"));
		TextWidget moneyText = TextWidget.Cast(transactionWidget.FindAnyWidget("MoneyText"));
		TextWidget commentText = TextWidget.Cast(transactionWidget.FindAnyWidget("CommentText"));

		dateText.SetText(EL_Utils.GetTimeFormatted(transaction.m_iDate));
		commentText.SetText(transaction.m_sComment);
		if (transaction.m_iAmount < 0)
		{
			int amount = transaction.m_iAmount * -1;
			moneyText.SetTextFormat("- $%1", EL_FormatUtils.DecimalSeperator(amount));
			moneyText.SetColor(Color.DarkRed);
		}
		else
		{
			moneyText.SetTextFormat("+ $%1", EL_FormatUtils.DecimalSeperator(transaction.m_iAmount));
			moneyText.SetColor(Color.DarkGreen);
		}
	}

    //------------------------------------------------------------------------------------------------
    override void OnMenuOpen()
    {
        m_wRoot = GetRootWidget();
		m_BankManager = EL_GlobalBankAccountManager.GetInstance();

		//Always add personal player account
		Widget defaultAccount = AddNewAccount(m_BankManager.GetLocalPlayerBankAccount());
		m_wActiveAccount = defaultAccount;

		SCR_ModularButtonComponent newAccountWidgetHandler = SCR_ModularButtonComponent.Cast(defaultAccount.FindHandler(SCR_ModularButtonComponent));
		newAccountWidgetHandler.SetToggled(true);

		//TODO: Add all other accounts that this player has access to..
		//m_BankManager.GetBankAccountWithAccess()....

		m_wCurrentCash = TextWidget.Cast(m_wRoot.FindAnyWidget("CurrentMoney"));
		m_wCurrentBalance = TextWidget.Cast(m_wActiveAccount.FindAnyWidget("CurrentBalance"));

		SCR_ButtonComponent depositButtonHandler = SCR_ButtonComponent.Cast(m_wRoot.FindAnyWidget("DepositButton").FindHandler(SCR_ButtonComponent));
		SCR_ButtonComponent withdrawButtonHandler = SCR_ButtonComponent.Cast(m_wRoot.FindAnyWidget("WithdrawButton").FindHandler(SCR_ButtonComponent));
		SCR_ButtonComponent transferButtonHandler = SCR_ButtonComponent.Cast(m_wRoot.FindAnyWidget("TransferButton").FindHandler(SCR_ButtonComponent));
		SCR_ButtonComponent exitButtonHandler = SCR_ButtonComponent.Cast(m_wRoot.FindAnyWidget("ExitButton").FindHandler(SCR_ButtonComponent));

		depositButtonHandler.m_OnClicked.Insert(OpenDepositMenu);
		withdrawButtonHandler.m_OnClicked.Insert(OpenWithdrawMenu);
		transferButtonHandler.m_OnClicked.Insert(OpenTransferMenu);
		exitButtonHandler.m_OnClicked.Insert(Close);
    }

	//------------------------------------------------------------------------------------------------
	override void OnMenuFocusGained()
	{
		UpdateCashText();
		//TODO: Update All accounts later here:
		UpdateBalanceText(m_wActiveAccount, m_BankManager.GetLocalPlayerBankAccount());
		UpdateTransactions();
	}
}