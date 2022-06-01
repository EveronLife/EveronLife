class EL_VehicleKeyComponentClass : ScriptComponentClass {

}

class EL_VehicleKeyComponent : ScriptComponent {
	
	[Attribute("Debug Identifier")]
	string m_DebugIdentifier;
	
	[RplProp()]
	string m_VehicleIdentifier;
	
	void EL_VehicleKeyComponent() {
		if (m_DebugIdentifier != "")
			m_VehicleIdentifier = m_DebugIdentifier;
	}
	
}