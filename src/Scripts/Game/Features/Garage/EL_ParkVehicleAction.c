class EL_ParkVehicleAction : ScriptedUserAction
{
	[Attribute(defvalue:"10", UIWidgets.EditBox, desc: "Garage Search Radius")]
	private float m_fGarageSearchRadius;

	private EL_GarageManagerComponent m_GarageManager;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		
		
		GetGame().GetWorld().QueryEntitiesBySphere(pOwnerEntity.GetOrigin(), m_fGarageSearchRadius, FindFirstGarage, FilterGarage);

		if (!m_GarageManager)
			Print("Garage not longer found!", LogLevel.WARNING);

		//Stop engine and eject passengers
		VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(pOwnerEntity.FindComponent(VehicleControllerComponent));
		vehicleController.StopEngine();
		
		SCR_BaseCompartmentManagerComponent vehicleComparmentManager = SCR_BaseCompartmentManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_BaseCompartmentManagerComponent));
		array<IEntity> occupants = {};
		vehicleComparmentManager.GetOccupants(occupants);
		
		foreach(IEntity occupant : occupants)
		{
			ChimeraCharacter character = ChimeraCharacter.Cast(occupant);
			if (!character)
				return;
			
			CharacterControllerComponent controller = character.GetCharacterController();
			if (!controller)
				continue;
			
			CompartmentAccessComponent access = character.GetCompartmentAccessComponent();
			if (!controller)
				continue;

			access.EjectOutOfVehicle();
			//controller.GetInputContext().SetVehicleCompartment(null);
		}
		
		//Save Vehicle
		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(pOwnerEntity.FindComponent(EL_PersistenceComponent));
		EL_EntitySaveDataBase saveData = persistence.Save();
		persistence.Detach();

		//Add to garage
		EL_CharacterOwnerComponent charOwnerComp = EL_CharacterOwnerComponent.Cast(GetOwner().FindComponent(EL_CharacterOwnerComponent));
		m_GarageManager.AddVehicle(saveData.GetId(), charOwnerComp.GetCharacterOwner());
		SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
	}


	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		//Check if user is owner of this vehicle
		EL_CharacterOwnerComponent charOwnerComp = EL_CharacterOwnerComponent.Cast(GetOwner().FindComponent(EL_CharacterOwnerComponent));
		if (EL_Utils.GetPlayerUID(user) != charOwnerComp.GetCharacterOwner())
		{
			SetCannotPerformReason("NOT OWNER: " + EL_Utils.GetPlayerUID(user) + " != " + charOwnerComp.GetCharacterOwner());
			//return false;
		}
		//Check if garage is nearby
		GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), m_fGarageSearchRadius, FindFirstGarage, FilterGarage);
		return (m_GarageManager);
 	}

	//------------------------------------------------------------------------------------------------
	bool FilterGarage(IEntity ent)
	{
		return (ent.FindComponent(EL_GarageManagerComponent));
	}

	//------------------------------------------------------------------------------------------------
	bool FindFirstGarage(IEntity ent)
	{
		m_GarageManager = EL_GarageManagerComponent.Cast(ent.FindComponent(EL_GarageManagerComponent));
		if (!m_GarageManager)
			return true; //Continue search

		return false; //Stop search
	}
}
