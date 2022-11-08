[EntityEditorProps(category: "EL/VehicleShop")]
class CurSpeedComponentClass : ScriptComponentClass
{
};

class CurSpeedComponent : ScriptComponent
{
	VehicleWheeledSimulation vehSim;
	SignalsManagerComponent signals;
	int m_iSpeedID;
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		Print(vehSim.GetSpeedKmh());
	}
	
	override void EOnInit(IEntity owner)
	{
		vehSim = VehicleWheeledSimulation.Cast(GetOwner().FindComponent(VehicleWheeledSimulation));
		signals = SignalsManagerComponent.Cast(owner.FindComponent(SignalsManagerComponent));

	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, false);
	}
	
}