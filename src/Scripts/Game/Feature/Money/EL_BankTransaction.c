class EL_BankTransaction
{
	int m_iAmount;
	int m_iDate;
	int m_iSourceAccount;
	int m_iTargetAccount;
	string m_sComment;

	//------------------------------------------------------------------------------------------------
	static EL_BankTransaction Create(int amount, int sourceAccount, int targetAccount, string comment = "", int dateFormat = 0)
	{
		EL_BankTransaction bankTransaction();

		bankTransaction.m_iAmount = amount;
		bankTransaction.m_iSourceAccount = sourceAccount;
		bankTransaction.m_iTargetAccount = targetAccount;
		bankTransaction.m_sComment = comment;

		if (dateFormat == 0)
			dateFormat = EL_Utils.GetUnixTime();
		bankTransaction.m_iDate = dateFormat;

		return bankTransaction;
	}

	//------------------------------------------------------------------------------------------------
	void EL_BankTransaction() {}
}
