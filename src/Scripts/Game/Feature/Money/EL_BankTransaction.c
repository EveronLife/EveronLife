class EL_BankTransaction
{
	int m_iAmount;
	string m_iDate;
	int m_iSourceAccount;
	int m_iTargetAccount;
	
	//------------------------------------------------------------------------------------------------
	static EL_BankTransaction Create(int amount, int sourceAccount, int targetAccount, string dateFormat = "")
	{
		if (dateFormat.IsEmpty())
			dateFormat = SCR_DateTimeHelper.GetDateTimeUTC();
		
		EL_BankTransaction bankTransaction();
		bankTransaction.m_iAmount = amount;
		bankTransaction.m_iDate = dateFormat; //Current: "yyyy-mm-dd hh:ii:ss" -> Unix timestamp here if avail
		bankTransaction.m_iSourceAccount = sourceAccount;
		bankTransaction.m_iTargetAccount = targetAccount;
		return bankTransaction;
	}
	
	//------------------------------------------------------------------------------------------------
	void EL_BankTransaction() {}	
}
