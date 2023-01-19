[ComponentEditorProps(category: "EveronLife/ShopItem", description: "Shop Item Reference")]
class EL_ShopItemComponentClass : ScriptComponentClass
{
}

class EL_ShopItemComponent : ScriptComponent
{
	[Attribute("{9CA1D6CE3F6914BA}Configs/ItemShop/EL_Shops.conf", UIWidgets.Object, "Shop config")]
	protected ref EL_ItemShopConfig m_ItemShopConfig;

	[Attribute("", UIWidgets.ResourcePickerThumbnail, "Item to sell / buy here", "et")]
	protected ResourceName m_ShopItemPrefab;

	protected EL_ItemPrice m_ShopItemPriceConfig;

	//------------------------------------------------------------------------------------------------
	EL_ItemPrice GetShopItemPriceConfig()
	{
		//Not init yet
		if (!m_ShopItemPriceConfig)
			m_ShopItemPriceConfig = FindPrefabShopItemConfig();
		return m_ShopItemPriceConfig;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetShopItemPrefab()
	{
		return m_ShopItemPrefab;
	}

	//------------------------------------------------------------------------------------------------
	protected EL_ItemPrice FindPrefabShopItemConfig()
	{
		if (!m_ItemShopConfig || !m_ShopItemPrefab)
			return null;

		foreach (EL_ItemPrice price : m_ItemShopConfig.m_aItemPriceConfigs)
		{
			if (price.m_Prefab == m_ShopItemPrefab)
					return price;
		}
		return null;
	}

	//------------------------------------------------------------------------------------------------
	//! Set Mesh to shop item mesh if not already set. This allows buying/selling from other meshes (eg. Apples from apple crate)
	override void EOnInit(IEntity owner)
	{
		if (!m_ShopItemPrefab)
		{
			Print(string.Format("[EL-ItemShop] Empty shop item (m_ShopItemPrefab not set).", m_ShopItemPrefab), LogLevel.WARNING);
			return;
		}

		//Create new mesh from m_ShopItemPrefab
		if (!owner.GetVObject())
		{
			VObject shopItemVObject = EL_PrefabUtils.GetPrefabVObject(m_ShopItemPrefab);
			if (!shopItemVObject)
			{
				Print(string.Format("[EL-ItemShop] No VObject found in %1", m_ShopItemPrefab), LogLevel.WARNING);
				return;
			}
			owner.SetObject(shopItemVObject, "");
		}

		//Create Hitbox
		Physics phys = Physics.CreateStatic(owner);
		if (!phys)
		{
			Print(string.Format("[EL-ItemShop] Unable to create Physics for %1 (No geometry embedded in m_ShopItemPrefab / mesh?)", owner.GetVObject()), LogLevel.WARNING);
			return;
		}

		phys.SetInteractionLayer(EPhysicsLayerPresets.FireGeo);
		owner.SetFlags(EntityFlags.ACTIVE, false);

		m_ShopItemPriceConfig = FindPrefabShopItemConfig();
		if (!m_ShopItemPriceConfig)
			Print("[EL-ItemShop] No price config found for prefab: " + m_ShopItemPrefab, LogLevel.WARNING);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, false);
	}
}
