[BaseContainerProps()]
class EL_CharacterSaveDataClass : EL_EntitySaveDataClass
{
	[Attribute(defvalue: "30", uiwidget: UIWidgets.Slider, desc: "Maximum time until the quickbar is synced after a change in SECONDS. Higher values reduce traffic.", params: "1 1000 1")]
	int m_iMaxQuickbackSaveTime;
}

[EL_DbName(EL_CharacterSaveData, "Character")]
class EL_CharacterSaveData : EL_EntitySaveData
{
}
