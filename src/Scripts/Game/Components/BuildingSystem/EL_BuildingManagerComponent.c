[ComponentEditorProps(category: "EveronLife/Game/Building")]
class EL_BuildingManagerComponentClass : ScriptComponentClass 
{
};

class EL_BuildingManagerComponent : ScriptComponent 
{

	[Attribute("", UIWidgets.ResourcePickerThumbnail, "Preview entity material", "emat", category: "Building Preview")]
	private ResourceName m_PreviewMaterial;
	
	[Attribute("", UIWidgets.ResourcePickerThumbnail, "Empty Base Preview Prefab", "et", category: "Building Preview")]
	private ResourceName m_BasePreviewPrefab;	
	
	private IEntity m_PreviewEntity;
	
	private CameraBase m_Cam;
	private vector m_PosInWorld;
	private InputManager m_InputManager;
	
	private bool canBuild = false;
	private EL_BuildingPerformerComponent m_BuildingPerformer;
	private EL_BuildingPerformerComponent m_LastBuildingPerformer;

	
	//------------------------------------------------------------------------------------------------
	void OnMouseLeftDown()
	{
		if (canBuild)
			EL_Utils.SpawnEntityPrefab(m_BuildingPerformer.m_BuildingPrefab, m_PosInWorld);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set preview mesh to new handheld item mesh and change to preview material.
	private void SetNewPreviewMesh()
	{
		m_PreviewEntity.SetObject(Resource.Load(m_BuildingPerformer.m_PreviewMesh).GetResource().ToVObject(), "");
		EL_Utils.ChangeEntityMaterial(m_PreviewEntity, m_PreviewMaterial);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get BuildingPerformerComponent from current held item
	private EL_BuildingPerformerComponent GetHeldBuildingPerformer(IEntity owner)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(owner);
		if (!gadgetManager)
			return null;
		
		EL_BuildingPerformerComponent buildingPerformer = EL_BuildingPerformerComponent.Cast(gadgetManager.GetHeldGadgetComponent());
		if (!buildingPerformer)
			return null;
		
		return buildingPerformer;
	}

	//------------------------------------------------------------------------------------------------
	//! Update preview entity origin to mouse pos
	private bool UpdatePreviewOrigin()
	{
		IEntity cursorTarget = m_Cam.GetCursorTargetWithPosition(m_PosInWorld);
		
		if (m_BuildingPerformer.m_GroundType == EGroundType.TERRAIN && cursorTarget.Type() != GenericTerrainEntity)
			return false;

		m_PreviewEntity.SetOrigin(m_PosInWorld);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if buildingPerformer is in hand and update visibility of preview entity
	private bool UpdateHandBuildingPerformer(IEntity owner) 
	{
		m_BuildingPerformer = GetHeldBuildingPerformer(owner);
		
		bool isVisible = (m_PreviewEntity.GetFlags() & EntityFlags.VISIBLE);
		
		//Added Performer
		if (m_BuildingPerformer && !isVisible)
			m_PreviewEntity.SetFlags(EntityFlags.VISIBLE, true);
		
		//Removed Performer
		if (!m_BuildingPerformer && isVisible)
			m_PreviewEntity.ClearFlags(EntityFlags.VISIBLE, true);
		
		return (m_BuildingPerformer);

	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		canBuild = false;
		m_Cam = GetGame().GetCameraManager().CurrentCamera();
		if (!m_PreviewEntity || !m_Cam)
			return;
		
		if (!UpdateHandBuildingPerformer(owner))
			return;

		//Cache current hand item to not set it every frame.
		if (m_BuildingPerformer != m_LastBuildingPerformer)
		{
			SetNewPreviewMesh();
			m_LastBuildingPerformer = m_BuildingPerformer;
		}
		
		canBuild = UpdatePreviewOrigin();
	}
		
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		GetOwner().SetFlags(EntityFlags.ACTIVE, true);
		
		m_InputManager = GetGame().GetInputManager();
		m_InputManager.AddActionListener("MouseLeft", EActionTrigger.DOWN, OnMouseLeftDown);
		
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_PreviewEntity = EL_Utils.SpawnEntityPrefab(m_BasePreviewPrefab, owner.GetOrigin());
		EL_Utils.ChangeEntityMaterial(m_PreviewEntity, m_PreviewMaterial);
		m_PreviewEntity.ClearFlags(EntityFlags.VISIBLE, true);
	}
		
	//------------------------------------------------------------------------------------------------
	//! Destructor -> Remove ActionListener
	void ~EL_BuildingManagerComponent()
	{
		m_InputManager.RemoveActionListener("MouseLeft", EActionTrigger.DOWN, OnMouseLeftDown);	
	}
};