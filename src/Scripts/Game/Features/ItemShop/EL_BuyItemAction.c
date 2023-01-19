class EL_BuyItemAction : ScriptedUserAction
{
	[Attribute("1", UIWidgets.EditBox, "Amount of items to buy at once")]
	protected int m_iBuyAmount;

	protected EL_ItemPrice m_ItemPriceConfig;
	protected ResourceName m_BuyablePrefab;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!EL_NetworkUtils.IsOwner(pOwnerEntity)) return;

		//Play pickup animation
		CharacterControllerComponent controller = CharacterControllerComponent.Cast(pUserEntity.FindComponent(CharacterControllerComponent));
		controller.TryPlayItemGesture(EItemGesture.EItemGesturePickUp);

		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		for (int i; i < m_iBuyAmount; i++)
		{
			EL_InventoryUtils.AddAmount(pUserEntity, m_BuyablePrefab, 1);
			EL_MoneyUtils.RemoveCash(pUserEntity, m_ItemPriceConfig.m_iBuyPrice);
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (m_ItemPriceConfig)
			outName = string.Format("Buy %1x %2 ($%3)", m_iBuyAmount, EL_UIInfoUtils.GetInfo(m_BuyablePrefab).GetName(), m_ItemPriceConfig.m_iBuyPrice * m_iBuyAmount);
		else
			outName = "Price Config not found!";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		if (!m_ItemPriceConfig || !m_BuyablePrefab)
			return false;

		//1. Check money
		if(EL_MoneyUtils.GetCash(user) < m_ItemPriceConfig.m_iBuyPrice * m_iBuyAmount)
		{
			SetCannotPerformReason("Can't afford");
			return false;
		}

		//2. Check free equipment slots
		IEntityComponentSource clothComponentSource = SCR_BaseContainerTools.FindComponentSource(Resource.Load(m_BuyablePrefab), BaseLoadoutClothComponent);
		if (clothComponentSource)
		{
			LoadoutAreaType areaType;
			clothComponentSource.Get("AreaType", areaType);
			BaseLoadoutManagerComponent loadoutManager = EL_ComponentFinder<BaseLoadoutManagerComponent>.Find(user);
			if (loadoutManager.IsAreaAvailable(areaType.Type()))
				return true;
		}

		//3. Check free inventory space
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager.CanInsertItem(GetOwner()))
		{
			SetCannotPerformReason("Inventory full");
			return false;
		}

		return true;
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
		m_ItemPriceConfig = shopItemComponent.GetShopItemPriceConfig();

		InventoryItemComponent invItem = EL_ComponentFinder<InventoryItemComponent>.Find(pOwnerEntity);
		invItem.SetAdditionalVolume(EL_PrefabUtils.GetPrefabItemVolume(m_BuyablePrefab));
		invItem.SetAdditionalWeight(EL_PrefabUtils.GetPrefabItemWeight(m_BuyablePrefab));
	}
}
