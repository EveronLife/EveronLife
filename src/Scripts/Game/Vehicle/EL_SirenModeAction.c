class SirenModeAction : ScriptedUserAction 
{
	[Attribute( uiwidget: UIWidgets.CheckBox)]
	protected bool m_bInteriorOnly;
	
	//! Will action be available for entities seated in pilot compartment only?
	[Attribute( uiwidget: UIWidgets.CheckBox, defvalue: "1")]
	protected bool m_bPilotOnly;
	
	[Attribute()]
	protected string m_Mode;
	
	protected EL_SirenManagerComponent m_Manager;
	
	protected IEntity m_pOwner;
	
	
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if(!m_Manager) m_Manager = EL_SirenManagerComponent.Cast(pOwnerEntity.FindComponent(EL_SirenManagerComponent));
		m_Manager.SetMode(m_Mode);
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		// See if character is in vehicle
		if (character && character.IsInVehicle())
		{
			// See if character is in "this" (owner) vehicle
			CompartmentAccessComponent compartmentAccess = CompartmentAccessComponent.Cast(character.FindComponent(CompartmentAccessComponent));
			if (compartmentAccess)
			{
				// Character is in compartment
				// that belongs to owner of this action
				BaseCompartmentSlot slot = compartmentAccess.GetCompartment();
				if (slot)
				{
					// Check pilot only condition
					if (m_bPilotOnly && !PilotCompartmentSlot.Cast(slot))
						return false;
					
					// Check interior only condition
					if (m_bInteriorOnly && slot.GetOwner() != m_pOwner)
						return false;
					
					return true;
				}
			}
			
			return false;
		}
		// We cannot be pilot nor interior, if we are not seated in vehicle at all.
		else if (m_bInteriorOnly || m_bPilotOnly)
			return false;
		
		return true;
	}
}