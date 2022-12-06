modded class SCR_InventoryMenuUI
{
	protected static SCR_InventoryMenuUI s_pELThisMenu;

	//------------------------------------------------------------------------------------------------
	static SCR_InventoryMenuUI EL_GetCurrentInstance()
	{
		return s_pELThisMenu;
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		s_pELThisMenu = this;
	}
}
