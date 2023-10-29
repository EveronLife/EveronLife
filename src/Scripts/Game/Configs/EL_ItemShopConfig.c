class EL_ShopTitle : BaseContainerCustomTitle
{
	//------------------------------------------------------------------------------------------------
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		int iBuyPrice;
		int iSellPrice;
		ResourceName prefab;
		source.Get("m_Prefab", prefab);
		source.Get("m_iBuyPrice", iBuyPrice);
		source.Get("m_iSellPrice", iSellPrice);
		string sName = FilePath.StripExtension(FilePath.StripPath(prefab.GetPath()));

		title = string.Format("%1: %2$ | %3$", sName, iBuyPrice, iSellPrice);

		return true;
	}
}

[BaseContainerProps(), EL_ShopTitle()]
class EL_ItemPrice
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, "Prefab for this item", "et")]
	ResourceName m_Prefab;
	[Attribute("-1", UIWidgets.EditBox, "Buy Price. -1 to disable buying")]
	int m_iBuyPrice;
	[Attribute("-1", UIWidgets.EditBox, "Sell Price. -1 to disable selling")]
	int m_iSellPrice;
	[Attribute("1", UIWidgets.CheckBox, "Allow / Enable actions that buy more than one item at once")]
	bool m_bAllowMultiBuy;
	[Attribute("1", UIWidgets.CheckBox, "Allow / Enable actions that sell more than one item at once")]
	bool m_bAllowMultiSell;
}

[BaseContainerProps(configRoot: true)]
class EL_ItemShopConfig : ScriptAndConfig
{
	[Attribute("", UIWidgets.Object, "")]
	ref array<ref EL_ItemPrice> m_aItemPriceConfigs;
}
