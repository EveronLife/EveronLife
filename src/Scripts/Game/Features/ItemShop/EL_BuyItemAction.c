class EL_BuyItemAction : ScriptedUserAction
{
	protected EL_Price m_ItemPriceConfig;
	protected ResourceName m_BuyablePrefab;
	protected IEntity m_BuyableEntity;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		EL_MoneyUtils.TrySell(pUserEntity, m_BuyablePrefab, m_ItemPriceConfig.m_iSellPrice);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (m_ItemPriceConfig)
			outName = string.Format("Buy %1 ($%2)", m_ItemPriceConfig.m_sName, m_ItemPriceConfig.m_iBuyPrice);
		else
			outName = "Price Config not found!";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		if (!m_ItemPriceConfig || !m_BuyablePrefab || !m_BuyableEntity)
			return false;

		string cannotPerformReason;
		bool canBuy = EL_MoneyUtils.CanBuy(user, m_BuyableEntity, m_ItemPriceConfig.m_iBuyPrice, cannotPerformReason);
		SetCannotPerformReason(cannotPerformReason);

		return canBuy;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		return (m_ItemPriceConfig.m_iBuyPrice != -1 && m_BuyablePrefab);
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		EL_ShopItemComponent shopItemComponent = EL_ShopItemComponent.Cast(pOwnerEntity.FindComponent(EL_ShopItemComponent));
		m_BuyablePrefab = shopItemComponent.GetShopItemPrefab();
		m_BuyableEntity = shopItemComponent.GetShopItemEntity();
		m_ItemPriceConfig = shopItemComponent.GetShopItemPriceConfig();
	}
}
