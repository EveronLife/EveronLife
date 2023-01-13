class EL_BankAccount
{
	protected int m_sAccountId;
	protected int m_iBalance;
	ref array<ref EL_BankTransaction> m_aTransactions = new array<ref EL_BankTransaction>();

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
	void NewTransaction(int amount, int sourceAccount)
	{
		EL_BankTransaction transaction = EL_BankTransaction.Create(amount, sourceAccount, m_sAccountId);
		m_aTransactions.Insert(transaction);
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
	bool TryDeposit(int amount)
	{
		if (EL_MoneyUtils.GetCash(SCR_PlayerController.GetLocalControlledEntity()) < amount)
			return false;

		m_iBalance += EL_MoneyUtils.RemoveCash(SCR_PlayerController.GetLocalControlledEntity(), amount);
		NewTransaction(amount, 0);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool TryWithdraw(int amount)
	{
		if (m_iBalance < amount)
			return false;

		m_iBalance -= EL_MoneyUtils.AddCash(SCR_PlayerController.GetLocalControlledEntity(), amount);
		NewTransaction(-amount, 0);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool TryTransfer(int amount, int targetAccount)
	{
		if (!TryWithdraw(amount))
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
	static EL_BankAccount Create(int id, int startBalance)
	{
		EL_BankAccount bankAccount();
		bankAccount.m_sAccountId = id;
		bankAccount.m_iBalance = startBalance;
		return bankAccount;
	}
	
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet) 
	{
		snapshot.Serialize(packet, 8);
		
		int count;
		snapshot.SerializeInt(count);
		packet.SerializeInt(count);
		for (int i = 0; i < count; i++)
		{
			int amount;
			snapshot.SerializeInt(amount);
			packet.SerializeInt(amount);
			
			string date;
			snapshot.SerializeBytes(date, 19);
			packet.SerializeString(date);
			
			int sourceAccountId;
			snapshot.SerializeInt(sourceAccountId);
			packet.SerializeInt(sourceAccountId);
			
			int targetAccountId;
			snapshot.SerializeInt(targetAccountId);
			packet.SerializeInt(targetAccountId);			
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{
		if (!snapshot.Serialize(packet, 8)) return false;
		
		int count;
		packet.SerializeInt(count);
		snapshot.SerializeInt(count);
		for (int i = 0; i < count; i++)
		{
			int amount;
			packet.SerializeInt(amount);
			snapshot.SerializeInt(amount);
			
			string date;
			packet.SerializeString(date);
			snapshot.SerializeBytes(date, 19);
			
			int sourceAccountId;
			packet.SerializeInt(sourceAccountId);
			snapshot.SerializeInt(sourceAccountId);
			
			int targetAccountId;
			packet.SerializeInt(targetAccountId);		
			snapshot.SerializeInt(targetAccountId);	
		}
		
		return true;
		
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx) 
	{		
		return lhs.CompareSnapshots(rhs, 12); //2 props + transaction array count = 3 * 4 byte
	}

	//------------------------------------------------------------------------------------------------
	static bool PropCompare(EL_BankAccount prop, SSnapSerializerBase snapshot, ScriptCtx ctx) 
	{
		int count = prop.m_aTransactions.Count();
		count = Math.Min(count, 5);
		
		return snapshot.Compare(prop.m_iBalance, 4) 
			&& snapshot.Compare(prop.m_sAccountId, 4)
			&& snapshot.Compare(prop.m_aTransactions, count + 1);
	}
			
	//------------------------------------------------------------------------------------------------
	static bool Extract(EL_BankAccount prop, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{
		snapshot.SerializeBytes(prop.m_iBalance, 4);
		snapshot.SerializeBytes(prop.m_sAccountId, 4);
		
		//Transactions limited to 5
		int count = prop.m_aTransactions.Count();
		count = Math.Min(count, 5);
		snapshot.SerializeBytes(count, 4);
		for (int i = 0; i < count; i++)
		{
			snapshot.SerializeBytes(prop.m_aTransactions[i].m_iAmount, 4);
			snapshot.SerializeBytes(prop.m_aTransactions[i].m_iDate, 19); //"yyyy-mm-dd hh:ii:ss" -> Unix timestamp?
			snapshot.SerializeBytes(prop.m_aTransactions[i].m_iSourceAccount, 4);
			snapshot.SerializeBytes(prop.m_aTransactions[i].m_iTargetAccount, 4);
		}

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, EL_BankAccount prop) 
	{
		snapshot.SerializeBytes(prop.m_iBalance, 4);
		snapshot.SerializeBytes(prop.m_sAccountId, 4);

		int count;
		snapshot.SerializeInt(count);
		prop.m_aTransactions = new array<ref EL_BankTransaction>();
		prop.m_aTransactions.Reserve(count);
		
		for (int i = 0; i < count; i++)
		{
			int amount;
			snapshot.SerializeInt(amount);
			string date;
			snapshot.SerializeBytes(date, 19);
			int sourceAccountId;
			snapshot.SerializeInt(sourceAccountId);
			int targetAccountId;
			snapshot.SerializeInt(targetAccountId);
			prop.m_aTransactions.Insert(EL_BankTransaction.Create(amount, sourceAccountId, targetAccountId, date));
		}
		
		return true;
	}
}
