[EntityEditorProps(category: "EveronLife/Core", description: "Global storage manager for ground item management. Requires the matching component to be attached")]
class EL_GlobalBankAccountManagerClass : GenericEntityClass
{
}

class EL_GlobalBankAccountManager : GenericEntity
{
	//<BankAccID, moneyAmount>
	protected ref map<string, int> m_mBankAccounts = new ref map<string, int>;
	
	protected static EL_GlobalBankAccountManager s_pInstance;
		
	//------------------------------------------------------------------------------------------------
	static EL_GlobalBankAccountManager GetInstance()
	{
		return s_pInstance;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetPlayerBankAccount(IEntity player)
	{
		string bankAccountId = "BankAccount-" + EL_Utils.GetPlayerUID(player);
		if (!m_mBankAccounts.Get(bankAccountId)) //SET DEFAULT VALUE
			m_mBankAccounts.Set(bankAccountId, 1000);
		return bankAccountId;
	}	
		
	//------------------------------------------------------------------------------------------------
	void SetBankAccounts(map<string, int> bankAccounts)
	{
		m_mBankAccounts = bankAccounts;
	}	
		
	//------------------------------------------------------------------------------------------------
	map<string, int> GetBankAccounts()
	{
		return m_mBankAccounts;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetBankAccountMoney(string bankAccountId)
	{
		int moneyAmount;
		m_mBankAccounts.Find(bankAccountId, moneyAmount);
		return moneyAmount;
	}
	
	//------------------------------------------------------------------------------------------------	
	void Deposit(string bankAccountId, int amount)
	{
		int curMoneyAmount;
		m_mBankAccounts.Find(bankAccountId, curMoneyAmount);
		m_mBankAccounts.Set(bankAccountId, curMoneyAmount + amount);
	}	
	
	//------------------------------------------------------------------------------------------------	
	bool TryWithdraw(string bankAccountId, int amount)
	{
		int curMoneyAmount;
		m_mBankAccounts.Find(bankAccountId, curMoneyAmount);
		if (curMoneyAmount < amount)
			return false;
		m_mBankAccounts.Set(bankAccountId, curMoneyAmount - amount);
		return true;
	}
		
	//------------------------------------------------------------------------------------------------	
	bool CanWithdraw(string bankAccountId, int amount)
	{
		int curMoneyAmount;
		m_mBankAccounts.Find(bankAccountId, curMoneyAmount);
		return (curMoneyAmount < amount);
	}
	
	//------------------------------------------------------------------------------------------------	
	void EL_GlobalBankAccountManager(IEntitySource src, IEntity parent) 
	{
		s_pInstance = this;
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, true);
	}
}