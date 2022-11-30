class EL_PriceTitle : BaseContainerCustomTitle
{
	//------------------------------------------------------------------------------------------------
    override bool _WB_GetCustomTitle(BaseContainer source, out string title)
    {
        int iBuyPrice;
        int iSellPrice;
        string sName = "";
        source.Get("m_sName", sName);
        source.Get("m_iBuyPrice", iBuyPrice);
        source.Get("m_iSellPrice", iSellPrice);

        title = string.Format("%1: %2$ | %3$", sName, iBuyPrice, iSellPrice);
        return true;
    }
};

[BaseContainerProps(), EL_PriceTitle()]
class EL_Price
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, "Prefab for this item", "et")]
	ResourceName m_Prefab;
	[Attribute("", UIWidgets.EditBox, "Action display name")]
	string m_sName;
	[Attribute("-1", UIWidgets.EditBox, "Buy Price. -1 to disable buying")]
	int m_iBuyPrice;
	[Attribute("-1", UIWidgets.EditBox, "Sell Price. -1 to disable selling")]
	int m_iSellPrice;
}

[BaseContainerProps(configRoot: true)]
class EL_PriceConfig : ScriptAndConfig
{
	[Attribute("", UIWidgets.Object, "")]
	ref array<ref EL_Price> m_aPriceConfigs;
}
