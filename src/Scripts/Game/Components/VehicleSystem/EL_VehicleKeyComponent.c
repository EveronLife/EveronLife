[ComponentEditorProps(category: "EveronLife/Feature/Vehicles", description: "Identifies which vehicle this key belongs to.")]
class EL_VehicleKeyComponentClass : ScriptComponentClass
{
}

class EL_VehicleKeyComponent : ScriptComponent
{
	[Attribute("")]
	protected string m_sDebugIdentifier;

	[RplProp()]
	protected string m_sVehicleIdentifier;

	//------------------------------------------------------------------------------------------------
	string GetVehicleIdentifier()
	{
		return m_sVehicleIdentifier;
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		ClearEventMask(owner, EntityEvent.INIT);

		if (m_sDebugIdentifier != "")
			m_sVehicleIdentifier = m_sDebugIdentifier;

		if (m_sVehicleIdentifier == string.Empty)
			return;

		InventoryItemComponent invComp = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
		if (!invComp)
			return;

		UIInfo uiInfo = invComp.GetUIInfo();
		if (uiInfo)
			uiInfo.SetName(string.Format("Vehicle Key [%1]", m_sVehicleIdentifier));
	}
}
