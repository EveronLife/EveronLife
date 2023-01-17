class EL_OpenBankMenuAction : ScriptedUserAction
{
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		Print("[EL-Bank] this guy opened it: " + GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetOwner()));

		EL_GlobalBankAccountManager.GetInstance().OpenBankMenu();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Open Bank";
		return true;
	}
	
}