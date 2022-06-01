[ComponentEditorProps(category: "EveronLife/Game/Building")]
class EL_BuildingPerformerComponentClass : SCR_GadgetComponentClass 
{
};	

enum EGroundType 
{
	TERRAIN,
	ANY
}

class EL_BuildingPerformerComponent : SCR_GadgetComponent 
{

	[Attribute("", UIWidgets.ResourcePickerThumbnail, "Building Prefab", "et", category: "Building")]
	ResourceName m_BuildingPrefab;
	
	[Attribute("TERRAIN", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EGroundType), desc: "Valid ground type to place", category: "Building")]
	EGroundType m_GroundType;
	
	ResourceName m_PreviewMesh;	
	
	//------------------------------------------------------------------------------------------------
	//! Get BuildingPrefab mesh and reference it here
	private void UpdatePreviewMesh() 
	{
		if (!m_BuildingPrefab)
			return;
		EL_Utils.GetPrefabMeshComponent(m_BuildingPrefab).Get("Object", m_PreviewMesh);		
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		UpdatePreviewMesh();
	}
	
};