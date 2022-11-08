class EL_PriceTitle : BaseContainerCustomTitle
{
    override bool _WB_GetCustomTitle(BaseContainer source, out string title)
    {
        int iBuyPrice = 0;
        int iSellPrice = 0;
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
	[Attribute("", UIWidgets.ResourcePickerThumbnail, "", "et")]
	ResourceName m_Prefab;
	[Attribute("", UIWidgets.EditBox)]
	string m_sName;
	[Attribute("0", UIWidgets.EditBox, "")]
	int m_iBuyPrice;
	[Attribute("0", UIWidgets.EditBox, "")]
	int m_iSellPrice;

}

[BaseContainerProps(configRoot: true)]
class EL_PriceConfig : ScriptAndConfig
{
	[Attribute("", UIWidgets.Object, "")]
	ref array<ref EL_Price> m_aPriceConfigs;
}