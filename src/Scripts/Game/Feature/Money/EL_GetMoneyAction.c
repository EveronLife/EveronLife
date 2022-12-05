class EL_WithdrawMoneyAction : ScriptedUserAction
{
	[Attribute("1", UIWidgets.Auto, "")]
	private int m_iAmount;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		string bankAccountId = EL_GlobalBankAccountManager.GetInstance().GetPlayerBankAccount(pOwnerEntity);
		if (!EL_GlobalBankAccountManager.GetInstance().TryWithdraw(bankAccountId, m_iAmount))
		{
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("You only have " + EL_GlobalBankAccountManager.GetInstance().GetBankAccountMoney(bankAccountId) + "$ left", "Bank", 20);
			return;
		}
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		EL_MoneyUtils.AddCash(inventoryManager, m_iAmount)
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		string bankAccountId = EL_GlobalBankAccountManager.GetInstance().GetPlayerBankAccount(user);
		
		return EL_GlobalBankAccountManager.GetInstance().CanWithdraw(bankAccountId, m_iAmount);
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
	}
}


class EL_DepositMoneyAction : ScriptedUserAction
{
	
	[Attribute("", UIWidgets.Auto, "")]
	private int m_iAmount;
		
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		string bankAccountId = EL_GlobalBankAccountManager.GetInstance().GetPlayerBankAccount(pOwnerEntity);
		EL_GlobalBankAccountManager.GetInstance().Deposit(bankAccountId, m_iAmount);
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		EL_MoneyUtils.RemoveCash(inventoryManager, m_iAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		return (EL_MoneyUtils.GetCash(inventoryManager) <= m_iAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Deposit " + m_iAmount + "$";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Not enough money to deposit");
	}
}
