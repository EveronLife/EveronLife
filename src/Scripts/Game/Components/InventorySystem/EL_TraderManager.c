class EL_TraderManagerComponentClass: GameComponentClass
{
};


class EL_TraderManagerComponent : GameComponent
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourceNamePicker, desc: "Item to Receive", category: "Trade" )]
	ResourceName m_ItemToReceive;
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourceNamePicker, desc: "Item to Give", category: "Trade" )]
	ResourceName m_ItemToGive;
}