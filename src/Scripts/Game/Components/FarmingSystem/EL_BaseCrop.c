[ComponentEditorProps(category: "EveronLife/Game/Farming")]
class EL_BaseCropClass : EL_BaseBuildingClass 
{
};	

class EL_BaseCrop : EL_BaseBuilding 
{
	bool m_IsGrown = false;
		
	//------------------------------------------------------------------------------------------------
	protected void Grow(IEntity owner, float timeSlice) 
	{
		m_IsGrown = true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{		
		super.EOnFrame(owner, timeSlice);
		if (!m_IsGrown)
			Grow(owner, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{	
		super.EOnInit(owner);
		
		owner.SetEventMask(EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
};