class EL_BankAccount
{
	protected string m_sOwnerUid;
	protected int m_iAccountId;
	protected int m_iBalance;
	ref array<ref EL_BankTransaction> m_aTransactions = new array<ref EL_BankTransaction>();

	protected static const int MAX_REPLICATED_TRANSACTIONS = 10;

	//------------------------------------------------------------------------------------------------
	static EL_BankAccount Create(string ownerUid, int accountId, int startBalance)
	{
		EL_BankAccount bankAccount();
		bankAccount.m_iBalance = startBalance;
		bankAccount.m_sOwnerUid = ownerUid;
		bankAccount.m_iAccountId = accountId;
		return bankAccount;
	}

	//------------------------------------------------------------------------------------------------
	int GetId()
	{
		return m_iAccountId;
	}

	//------------------------------------------------------------------------------------------------
	string GetAccountOwnerUid()
	{
		return m_sOwnerUid;
	}

	//------------------------------------------------------------------------------------------------
	IEntity GetAccountOwner()
	{
		return EL_Utils.GetPlayerByUID(m_sOwnerUid);
	}

	//------------------------------------------------------------------------------------------------
	int GetBalance()
	{
		return m_iBalance;
	}

	//------------------------------------------------------------------------------------------------
	void NewTransaction(int amount, int sourceAccountId, string comment)
	{
		Print("[EL-Bank] New transaction from account: " + sourceAccountId + " | Amount: " + amount);
		EL_BankTransaction transaction = EL_BankTransaction.Create(amount, 0, sourceAccountId, comment);
		m_aTransactions.InsertAt(transaction, 0);
	}

	//------------------------------------------------------------------------------------------------
	void AddBalance(int amount)
	{
		m_iBalance += amount;
	}

	//------------------------------------------------------------------------------------------------
	void RemoveBalance(int amount)
	{
		m_iBalance -= amount;
	}

	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	bool TryDeposit(int amount, string comment)
	{
		Print(string.Format("[EL-Bank] %1 Tried to deposit $ %2 with comment: '%3'", m_sOwnerUid, amount, comment));
		if (EL_MoneyUtils.GetCash(GetAccountOwner()) < amount)
			return false;
		int amountRemoved = EL_MoneyUtils.RemoveCash(GetAccountOwner(), amount);
		if (amountRemoved == 0)
			return false;
		m_iBalance += amountRemoved;
		NewTransaction(amount, 0, comment);
		Print(string.Format("[EL-Bank] %1 deposited $ %2 with comment: '%3'", m_sOwnerUid, amount, comment));
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	bool TryWithdraw(int amount, string comment)
	{
		Print(string.Format("[EL-Bank] %1 Tried to withdraw $ %2 with comment: '%3'", m_sOwnerUid, amount, comment));
		if (m_iBalance < amount)
			return false;

		m_iBalance -= EL_MoneyUtils.AddCash(GetAccountOwner(), amount);
		NewTransaction(-amount, 0, comment);
		Print(string.Format("[EL-Bank] %1 withdrew $ %2 with comment: '%3'", m_sOwnerUid, amount, comment));
		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool TryTransfer(int amount, int targetAccount, string comment)
	{
		if (!TryWithdraw(amount, comment))
			return false;

		//EL_GlobalBankAccountManager.GetInstance().GetPlayerBankAccount(targetAccount).AddBalance(amount);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool CanWithdraw(int amount)
	{
		return (m_iBalance >= amount);
	}



	//------------------------------------------------------------------------------------------------
	static bool Extract(EL_BankAccount prop, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		snapshot.SerializeInt(prop.m_iAccountId);
		snapshot.SerializeInt(prop.m_iBalance);

		//Transactions limited to 5
		int count = prop.m_aTransactions.Count();
		count = Math.Min(count, MAX_REPLICATED_TRANSACTIONS);
		snapshot.SerializeInt(count);
		for (int i = 0; i < count; i++)
		{
			snapshot.SerializeInt(prop.m_aTransactions[i].m_iAmount);
			snapshot.SerializeInt(prop.m_aTransactions[i].m_iDate);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		int accountId;
		snapshot.SerializeInt(accountId);
		packet.SerializeInt(accountId);

		int balance;
		snapshot.SerializeInt(balance);
		packet.SerializeInt(balance);

		int count;
		snapshot.SerializeInt(count);
		packet.SerializeInt(count);
		for (int i = 0; i < count; i++)
		{
			int amount;
			snapshot.SerializeInt(amount);
			packet.SerializeInt(amount);

			int dateAsInt;
			snapshot.SerializeInt(dateAsInt);
			packet.SerializeInt(dateAsInt);
		}
	}


	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		int accountId;
		packet.SerializeInt(accountId);
		snapshot.SerializeInt(accountId);

		int balance;
		packet.SerializeInt(balance);
		snapshot.SerializeInt(balance);

		int count;
		packet.SerializeInt(count);
		snapshot.SerializeInt(count);

		for (int i = 0; i < count; i++)
		{
			int amount;
			packet.SerializeInt(amount);
			snapshot.SerializeInt(amount);

			int dateAsInt
			packet.SerializeInt(dateAsInt);
			snapshot.SerializeInt(dateAsInt);
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, EL_BankAccount prop)
	{
		snapshot.SerializeInt(prop.m_iAccountId);
		snapshot.SerializeInt(prop.m_iBalance);

		int count;
		snapshot.SerializeInt(count);
		prop.m_aTransactions.Clear();
		prop.m_aTransactions.Reserve(count);
		for (int i = 0; i < count; i++)
		{
			int amount;
			snapshot.SerializeInt(amount);
			int dateAsInt;
			snapshot.SerializeInt(dateAsInt);

			prop.m_aTransactions.Insert(EL_BankTransaction.Create(amount, 0, 0, "", dateAsInt));
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
	{
		if (!lhs.CompareSnapshots(rhs, 4))
			return false;

		if (!lhs.CompareSnapshots(rhs, 4))
			return false;

		int lhsCount;
		int rhsCount;
		lhs.SerializeInt(lhsCount);
		rhs.SerializeInt(rhsCount);
		if (lhsCount != rhsCount)
			return false;

		for (int i = 0; i < lhsCount; i++)
		{
			if (!lhs.CompareSnapshots(rhs, 4))
				return false;

			if (!lhs.CompareSnapshots(rhs, 4))
				return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool PropCompare(EL_BankAccount prop, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		if (!snapshot.CompareInt(prop.m_iAccountId))
			return false;
		if (!snapshot.CompareInt(prop.m_iBalance))
			return false;

		int count = prop.m_aTransactions.Count();
		count = Math.Min(count, MAX_REPLICATED_TRANSACTIONS);
		if (!snapshot.CompareInt(count))
			return false;

		for (int i = 0; i < count; i++)
		{
			if (!snapshot.CompareInt(prop.m_aTransactions[i].m_iAmount))
				return false;
			if (!snapshot.CompareInt(prop.m_aTransactions[i].m_iDate))
				return false;
		}
		return true;
	}
}
