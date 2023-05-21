[EntityEditorProps(category: "EveronLife/Core/Persistence", description: "Proxy to assign name to base scene entities.")]
class EL_BaseSceneNameProxyEntityClass : GenericEntityClass
{
};

class EL_BaseSceneNameProxyEntity : GenericEntity
{
	[Attribute(uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Target prefab to find closest to proxy position.")]
	protected ResourceName m_rTarget;

	#ifdef WORKBENCH
	static ref array<EL_BaseSceneNameProxyEntity> s_aAllProxies = {};

	static EL_BaseSceneNameProxyEntity s_pSelectedProxy;

	IEntity m_pTarget;
	#endif

	//------------------------------------------------------------------------------------------------
	protected void EL_BaseSceneNameProxyEntity(IEntitySource src, IEntity parent)
	{
		FindTarget(src);

		if (!GetWorld().IsEditMode())
		{
			delete this;
			return;
		}

		#ifdef WORKBENCH
		s_aAllProxies.Insert(this);
		#endif
	}

	//------------------------------------------------------------------------------------------------
	#ifdef WORKBENCH
	protected void ~EL_BaseSceneNameProxyEntity()
	{
		s_aAllProxies.RemoveItem(this);
	}
	#endif

	//------------------------------------------------------------------------------------------------
	protected void FindTarget(BaseContainer src = null, float radius = 1.0)
	{
		#ifdef WORKBENCH
		m_pTarget = null;
		#endif
		if (src && (!src.Get("m_rTarget", m_rTarget) || !m_rTarget))
		{
			#ifdef WORKBENCH
			if (!_WB_GetEditorAPI().IsDoingEditAction())
			#endif
				Debug.Error(string.Format("Base scene name proxy '%1' without assigned target prefab. Fix or delete proxy entity!", GetName()));
		}
		else
		{
			if (GetWorld().QueryEntitiesBySphere(GetOrigin(), radius, WorldSearchCallback))
				Debug.Error(string.Format("Base scene name proxy '%1' failed to find target prefab '%2' in %3 meter radius. Fix or delete proxy entity!", GetName(), m_rTarget, radius));
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool WorldSearchCallback(IEntity ent)
	{
		if (EL_Utils.GetPrefabName(ent) == m_rTarget)
		{
			#ifdef WORKBENCH
			if (GetWorld().IsEditMode())
			{
				m_pTarget = ent;
				return false;
			}
			#endif

			ent.SetName(GetName());
			return false;
		}
		return true;
	}

	#ifdef WORKBENCH
	static EL_BaseSceneNameProxyEntity GetProxyForBaseSceneEntity(notnull IEntity baseSceneEntity)
	{
		foreach (EL_BaseSceneNameProxyEntity proxy : s_aAllProxies)
		{
			if (proxy.m_pTarget == baseSceneEntity)
				return proxy;
		}

		return null;
	}

	//------------------------------------------------------------------------------------------------
	override bool _WB_CanCopy(IEntitySource src)
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool _WB_OnKeyChanged(BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent)
	{
		FindTarget(src);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override array<ref WB_UIMenuItem> _WB_GetContextMenuItems()
	{
		array<ref WB_UIMenuItem> items();

		if (m_pTarget)
		{
			items.Insert(new WB_UIMenuItem("Go to proxy target", 0));
		}
		else if (m_rTarget)
		{
			items.Insert(new WB_UIMenuItem("Try autofix proxy target", 1));
			items.Insert(new WB_UIMenuItem("Remember proxy target", 2));
		}

		return items;
	}

	//------------------------------------------------------------------------------------------------
	override void _WB_OnContextMenu(int id)
	{
		WorldEditorAPI worldEditorApi = _WB_GetEditorAPI();

		if (id == 2)
		{
			s_pSelectedProxy = this;
			return;
		}

		if (id == 1)
		{
			FindTarget(radius: 50);
			if (!m_pTarget)
				return;

			worldEditorApi.BeginEntityAction("BaseSceneNameProxyEntity__AutoFix");
			_WB_GetEditorAPI().ModifyEntityKey(this, "coords", m_pTarget.GetOrigin().ToString(false));
			worldEditorApi.EndEntityAction();
		}

		worldEditorApi.SetEntitySelection(m_pTarget);
		worldEditorApi.UpdateSelectionGui();
	}

	//------------------------------------------------------------------------------------------------
	override void _WB_AfterWorldUpdate(float timeSlice)
	{
		IEntity selected = _WB_GetEditorAPI().GetSelectedEntity();
		if (!m_pTarget || (selected != this && selected != m_pTarget))
			return;

		//Shape.CreateSphere(Color.PINK, ShapeFlags.ONCE | ShapeFlags.WIREFRAME | ShapeFlags.NOZBUFFER, GetOrigin(), 0.5);

		vector position = m_pTarget.GetOrigin();
		vector bboxMin, bboxMax;
		m_pTarget.GetWorldBounds(bboxMin, bboxMax);
		DebugTextWorldSpace.Create(
			GetWorld(),
			GetName(),
			DebugTextFlags.ONCE | DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA,
			position[0],
			position[1] + bboxMax[1] + 0.5,
			position[2],
			20.0,
			Color.PINK);
	}
	#endif
};
