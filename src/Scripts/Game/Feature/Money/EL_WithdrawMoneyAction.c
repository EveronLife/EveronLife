class EL_WithdrawMoneyAction : ScriptedUserAction
{
	[Attribute("1", UIWidgets.Auto, "Amount to withdraw (-1 for all)")]
	protected int m_iAmount;
	
	protected EL_GlobalBankAccountManager m_BankManager;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		string bankAccountId = m_BankManager.GetPlayerBankAccount(pUserEntity);
		if (m_iAmount == -1)
			m_iAmount = m_BankManager.GetBankAccountMoney(bankAccountId);
		
		if(m_BankManager.TryWithdraw(bankAccountId, m_iAmount))
			EL_MoneyUtils.AddCash(pUserEntity, m_iAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		if (m_iAmount == -1)
		{
			string bankAccountId = m_BankManager.GetPlayerBankAccount(user);
			m_iAmount = m_BankManager.GetBankAccountMoney(bankAccountId);
		}
		
		string bankAccountId = m_BankManager.GetPlayerBankAccount(user);
		return (m_iAmount != 0 && m_BankManager.CanWithdraw(bankAccountId, m_iAmount));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Withdraw " + m_iAmount + "$";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Not enough money in bank");
		m_BankManager = EL_GlobalBankAccountManager.GetInstance();
	}
}
