class EL_BankTransaction
{
	int m_iAmount;
	int m_iDate;
	int m_iSourceAccount;
	int m_iTargetAccount;
	
	//------------------------------------------------------------------------------------------------
	static EL_BankTransaction Create(int amount, int sourceAccount, int targetAccount, int dateFormat = 0)
	{
		if (dateFormat == 0)
			dateFormat = EL_Utils.GetUnixTime();
		Print(EL_Utils.GetTimeFormatted(dateFormat));
		EL_BankTransaction bankTransaction();
		bankTransaction.m_iAmount = amount;
		bankTransaction.m_iDate = dateFormat;
		bankTransaction.m_iSourceAccount = sourceAccount;
		bankTransaction.m_iTargetAccount = targetAccount;
		return bankTransaction;
	}
	
	//------------------------------------------------------------------------------------------------
	void EL_BankTransaction() {}	
}
