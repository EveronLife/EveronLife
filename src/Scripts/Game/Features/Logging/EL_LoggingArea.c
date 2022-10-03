[EntityEditorProps(category: "EveronLife/Logging", description: "Logging Area", color: "251 91 0 255", dynamicBox: true)]
class EL_LoggingAreaClass: GenericEntityClass
{
};

class EL_LoggingArea : GenericEntity
{
	[Attribute("10 10 10", desc: "Logging area size", category: "Logging Area")]
	private vector m_vSize;

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner) 
	{
		GetWorld().QueryEntitiesByAABB(GetOrigin() - m_vSize, GetOrigin() + m_vSize, AddEntity, FilterTrees);
	}
	
	//------------------------------------------------------------------------------------------------
	bool AddEntity(IEntity ent)
	{
		SCR_DestructionMultiPhaseComponent destructionManager = SCR_DestructionMultiPhaseComponent.Cast(ent.FindComponent(SCR_DestructionMultiPhaseComponent));
		if (!destructionManager)
			return true;
		destructionManager.EnableDamageHandling(true);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool FilterTrees(IEntity ent) 
	{
		return (ent.Type() == Tree);
	}
	
	//------------------------------------------------------------------------------------------------
	void EL_LoggingArea(IEntitySource src, IEntity parent) 
	{
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, true);
	}

#ifdef WORKBENCH
	override void _WB_GetBoundBox(inout vector min, inout vector max, IEntitySource src)
	{
		min = -m_vSize;
		max = m_vSize;
	}
#endif
	
};
