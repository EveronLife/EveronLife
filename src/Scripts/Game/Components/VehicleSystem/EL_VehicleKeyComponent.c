class EL_VehicleKeyComponentClass : GameComponentClass {

}

class EL_VehicleKeyComponent : GameComponent {
	
	[Attribute("Debug Identifier")]
	string m_DebugIdentifier;
	
	[RplProp()]
	string m_VehicleIdentifier;
	
	void EL_VehicleKeyComponent() {
		if (m_DebugIdentifier != "")
			m_VehicleIdentifier = m_DebugIdentifier;
	}
	
}