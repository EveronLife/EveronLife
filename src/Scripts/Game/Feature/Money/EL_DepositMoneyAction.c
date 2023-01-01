class EL_DepositMoneyAction : ScriptedUserAction
{
	[Attribute("1", UIWidgets.Auto, "Amount to deposit (-1 for all)")]
	protected int m_iAmount;
	
	protected EL_GlobalBankAccountManager m_BankManager;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		string bankAccountId = m_BankManager.GetPlayerBankAccount(pUserEntity);
		if (m_iAmount == -1)
			m_iAmount = EL_MoneyUtils.GetCash(pUserEntity);

		m_BankManager.Deposit(bankAccountId, m_iAmount);
		EL_MoneyUtils.RemoveCash(pUserEntity, m_iAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	//! FIX THIS WITH outName. make another m_iAmount temp
	override bool CanBePerformedScript(IEntity user)
 	{
		return (m_iAmount == -1 || EL_MoneyUtils.GetCash(user) >= m_iAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Deposit " + m_iAmount + "$";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Not enough money to deposit");
		m_BankManager = EL_GlobalBankAccountManager.GetInstance();
	}
}