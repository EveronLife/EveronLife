[EL_DbName(EL_BankAccountSaveData, "BankAccount"), BaseContainerProps()]
class EL_BankAccountSaveData : EL_EntitySaveDataBase
{
	protected ref map<string, int> m_mBankAccounts = new ref map<string, int>;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull IEntity worldEntity)
	{
		//Meta data
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		SetId(persistenceComponent.GetPersistentId());
		m_iLastSaved = persistenceComponent.GetLastSaved();

		//Actual Data
		m_mBankAccounts = EL_GlobalBankAccountManager.GetInstance().GetBankAccounts();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull IEntity worldEntity)
	{
		EL_GlobalBankAccountManager.GetInstance().SetBankAccounts(m_mBankAccounts);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		//Meta data
		saveContext.WriteValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		saveContext.WriteValue("m_sId", GetId());
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);

		//Actual Data
		saveContext.WriteValue("m_mBankAccounts", m_mBankAccounts);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		//Meta data
		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		string id;
		loadContext.ReadValue("m_sId", id);
		SetId(id);
		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);

		//Actual Data
		loadContext.ReadValue("m_mBankAccounts", m_mBankAccounts);

		return true;
	}
}
