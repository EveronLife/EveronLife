class EL_BankAccount
{
	protected int m_sAccountId;
	protected int m_iBalance;
	
	//------------------------------------------------------------------------------------------------	
	int GetId()
	{
		return m_sAccountId;
	}
	
	//------------------------------------------------------------------------------------------------	
	int GetBalance()
	{
		return m_iBalance;
	}
		
	//------------------------------------------------------------------------------------------------	
	void AddBalance(int amount)
	{
		m_iBalance += EL_MoneyUtils.AddCash(SCR_PlayerController.GetLocalControlledEntity(), amount);
	}	
		
	//------------------------------------------------------------------------------------------------	
	bool TryDeposit(int amount)
	{
		if (EL_MoneyUtils.GetCash(SCR_PlayerController.GetLocalControlledEntity()) < amount)
			return false;
		
		m_iBalance += EL_MoneyUtils.RemoveCash(SCR_PlayerController.GetLocalControlledEntity(), amount);
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------	
	bool TryWithdraw(int amount)
	{
		if (m_iBalance < amount)
			return false;
		
		m_iBalance -= EL_MoneyUtils.AddCash(SCR_PlayerController.GetLocalControlledEntity(), amount);		
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------	
	bool TryTransfer(int amount, int targetAccount)
	{
		if (!TryWithdraw(amount))
			return false;
		
		EL_GlobalBankAccountManager.GetInstance().GetPlayerBankAccount(targetAccount).AddBalance(amount);
		
		return true;
	}
		
	//------------------------------------------------------------------------------------------------	
	bool CanWithdraw(int amount)
	{
		return (m_iBalance >= amount);
	}
	
	void EL_BankAccount(int id, int startBalance)
	{
		m_sAccountId = id;
		m_iBalance = startBalance;
	}
}