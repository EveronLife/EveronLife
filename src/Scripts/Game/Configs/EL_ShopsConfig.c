[BaseContainerProps(configRoot: true)]
class EL_ShopsConfig : ScriptAndConfig
{
	[Attribute("", UIWidgets.Object, "", "conf")]
	ref array<ref EL_ShopConfig> m_aShopConfigs;
}
