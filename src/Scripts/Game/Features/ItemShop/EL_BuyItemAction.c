class EL_BuyItemAction : ScriptedUserAction
{
	[Attribute("1", UIWidgets.EditBox, "Amount of items to buy at once")]
	protected int m_iBuyAmount;

	protected EL_Price m_ItemPriceConfig;
	protected ResourceName m_BuyablePrefab;
	protected IEntity m_BuyableEntity;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!EL_NetworkUtils.IsOwner(pOwnerEntity)) return;

		EL_MoneyUtils.TryBuy(pUserEntity, m_BuyablePrefab, m_ItemPriceConfig.m_iBuyPrice, m_iBuyAmount);
		CharacterControllerComponent controller = CharacterControllerComponent.Cast(pUserEntity.FindComponent(CharacterControllerComponent));
		controller.TryPlayItemGesture(EItemGesture.EItemGesturePickUp);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (m_ItemPriceConfig)
			outName = string.Format("Buy %1x %2 ($%3)", m_iBuyAmount, m_ItemPriceConfig.m_sName, m_ItemPriceConfig.m_iBuyPrice * m_iBuyAmount);
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
		bool canBuy = EL_MoneyUtils.CanBuy(user, m_BuyablePrefab, m_ItemPriceConfig.m_iBuyPrice * m_iBuyAmount, cannotPerformReason);
		SetCannotPerformReason(cannotPerformReason);

		return canBuy;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		return (
			m_ItemPriceConfig &&
			m_ItemPriceConfig.m_iBuyPrice != -1 &&
			m_iBuyAmount != 0 &&
			(m_iBuyAmount == 1 || m_ItemPriceConfig.m_bAllowMultiBuy) &&
			m_BuyablePrefab
		);
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
