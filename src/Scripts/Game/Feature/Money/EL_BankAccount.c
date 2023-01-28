class EL_BankAccount
{
	protected int m_sAccountId;
	protected int m_iBalance;
	static ref array<ref EL_BankTransaction> m_aTransactions = new array<ref EL_BankTransaction>();

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
	bool TryDeposit(int amount)
	{
		if (EL_MoneyUtils.GetCash(SCR_PlayerController.GetLocalControlledEntity()) < amount)
			return false;
		
		//TODO: wtf client cant do that! Authority only ...
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
	static bool Extract(EL_BankAccount prop, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{
		snapshot.SerializeInt(prop.m_iBalance);
		snapshot.SerializeInt(prop.m_sAccountId);
		/*
		//Transactions limited to 5
		int count = prop.m_aTransactions.Count();
		count = Math.Min(count, 5);
		snapshot.SerializeInt(count);
		for (int i = 0; i < count; i++)
		{
			snapshot.SerializeInt(prop.m_aTransactions[i].m_iAmount);
			snapshot.SerializeInt(prop.m_aTransactions[i].m_iDate);
			snapshot.SerializeInt(prop.m_aTransactions[i].m_iSourceAccount);
			snapshot.SerializeInt(prop.m_aTransactions[i].m_iTargetAccount);
		}
		*/
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet) 
	{
		snapshot.Serialize(packet, 8);
		/*
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

			int sourceAccountId;
			snapshot.SerializeInt(sourceAccountId);
			packet.SerializeInt(sourceAccountId);
			
			int targetAccountId;
			snapshot.SerializeInt(targetAccountId);
			packet.SerializeInt(targetAccountId);			
		}
		*/
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{
		if (!snapshot.Serialize(packet, 8)) return false;
		/*
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
						
			int sourceAccountId;
			packet.SerializeInt(sourceAccountId);
			snapshot.SerializeInt(sourceAccountId);
			
			int targetAccountId;
			packet.SerializeInt(targetAccountId);		
			snapshot.SerializeInt(targetAccountId);	
		}
		*/
		
		return true;
		
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, EL_BankAccount prop) 
	{
		snapshot.SerializeInt(prop.m_iBalance);
		snapshot.SerializeInt(prop.m_sAccountId);
/*
		int count;
		snapshot.SerializeInt(count);
		prop.m_aTransactions = new array<ref EL_BankTransaction>();
		prop.m_aTransactions.Reserve(count);
		
		for (int i = 0; i < count; i++)
		{
			int amount;
			snapshot.SerializeInt(amount);
			int length;
			snapshot.SerializeInt(length);
			int dateAsInt;
			snapshot.SerializeInt(dateAsInt);
			int sourceAccountId;
			snapshot.SerializeInt(sourceAccountId);
			int targetAccountId;
			snapshot.SerializeInt(targetAccountId);
			
			prop.m_aTransactions.InsertAt(EL_BankTransaction.Create(amount, sourceAccountId, targetAccountId, dateAsInt), 0);
		}
		*/
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx) 
	{		
		return lhs.CompareSnapshots(rhs, 8); //2 props + transaction array count = 3 * 4 byte (int)
	}

	//------------------------------------------------------------------------------------------------
	static bool PropCompare(EL_BankAccount prop, SSnapSerializerBase snapshot, ScriptCtx ctx) 
	{
		//int count = prop.m_aTransactions.Count();
		//count = Math.Min(count, 5);
		
		return snapshot.Compare(prop.m_iBalance, 4) 
			&& snapshot.Compare(prop.m_sAccountId, 4);
			//&& snapshot.Compare(prop.m_aTransactions, count + 1);
	}
}
