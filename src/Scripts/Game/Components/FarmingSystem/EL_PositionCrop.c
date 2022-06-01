[ComponentEditorProps(category: "EveronLife/Game/Farming")]
class EL_PositionCropClass : EL_BaseCropClass 
{
};	


class EL_PositionCrop : EL_BaseCrop
{
	[Attribute("1", UIWidgets.Auto, "Time to fully grow (s)", category: "Crop")]
	private float growTime;
	[Attribute("", UIWidgets.Auto, "How high the crop moves Y Pos", category: "Crop")]
	private float totalYIncrease;
	
	private vector startPos;
	private float elapsedTime = 0;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		startPos = owner.GetOrigin();
		startPos[1] = startPos[1] - totalYIncrease;
		owner.SetOrigin(startPos);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Override Grow method to add Y increase
	override void Grow(IEntity owner, float timeSlice) 
	{
		if (m_IsGrown)
			return;
		
		vector newPos = startPos;
		if (elapsedTime < growTime)
		{
			newPos = startPos;
			newPos[1] = newPos[1] + (elapsedTime / growTime) * totalYIncrease;
			elapsedTime += timeSlice;
		} else 
		{
			//Growing done -> Set pos to total increase
			newPos[1] = startPos[1] + totalYIncrease;
			m_IsGrown = true;
		}
		
		owner.SetOrigin(newPos);
	}	

};