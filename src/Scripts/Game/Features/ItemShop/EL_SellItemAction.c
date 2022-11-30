class EL_SellItemAction : ScriptedUserAction
{
	ResourceName m_SellablePrefab;

	private const ResourceName MONEY_PREFAB = "{FDEE11D818A4C675}Prefabs/Items/Money/DollarBill.et";
	private EL_Price m_ItemPriceConfig;
	ref SCR_PrefabNamePredicate m_pPrefabNamePredicate = new SCR_PrefabNamePredicate();


	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		EL_MoneyUtils.TrySell(pUserEntity, m_SellablePrefab, m_ItemPriceConfig.m_iSellPrice);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (m_ItemPriceConfig)
			outName = string.Format("Sell %1 ($%2)", m_ItemPriceConfig.m_sName, m_ItemPriceConfig.m_iSellPrice);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		return (m_SellablePrefab && m_ItemPriceConfig && inventoryManager.FindItem(m_pPrefabNamePredicate));
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
 	{
		return (m_ItemPriceConfig.m_iSellPrice != -1 && m_SellablePrefab);
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Item not found");

		EL_ShopItemComponent shopItemComponent = EL_ShopItemComponent.Cast(pOwnerEntity.FindComponent(EL_ShopItemComponent));
		m_SellablePrefab = shopItemComponent.GetShopItemPrefab();
		m_ItemPriceConfig = shopItemComponent.GetShopItemPriceConfig();
		m_pPrefabNamePredicate.prefabName = m_SellablePrefab;

	}

}
