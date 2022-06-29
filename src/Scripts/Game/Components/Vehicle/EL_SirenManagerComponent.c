class EL_SirenManagerComponentClass : ScriptComponentClass
{

}

class EL_SirenManagerComponent : ScriptComponent
{
	[Attribute()]
	protected ref array<ref EL_LightAnimation> m_Animations;
	
	
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.SIMULATE);
	}
	
	
	void Register(EL_LightComponent light)
	{
		foreach(EL_LightAnimation animation : m_Animations)
		{
			animation.Insert(light);
		}
	}
	
	
	override event protected void EOnSimulate(IEntity owner, float timeSlice)
	{
		m_Animations[0].Tick(timeSlice);
	}
}