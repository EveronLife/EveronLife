[ComponentEditorProps(category: "EveronLife/Game/Building")]
class EL_BaseBuildingClass : GenericEntityClass 
{
};	

enum EBuildingType
{
	CROP
};

class EL_BaseBuilding : GenericEntity 
{
	[Attribute("", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EBuildingType), desc: "Placeable Type")]
	EBuildingType m_buildingType;
};