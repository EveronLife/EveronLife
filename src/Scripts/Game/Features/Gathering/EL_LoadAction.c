class EL_LoadAction : ScriptedUserAction
{
		
	private IEntity m_ValidVehicle;
	private SCR_VehicleInventoryStorageManagerComponent m_InventoryManager;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		//Spawn item
		bool loadSuccess = m_InventoryManager.TryInsertItem(pOwnerEntity);
		if (loadSuccess)
		{
			Print("Loaded into " + m_InventoryManager.GetOwner());
		} else
		{
			Print("Unknown error loading..");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Load into vehicle");
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		SetCannotPerformReason("No nearby vehicle");
		
		GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), 10.0, ContinueSearch, FilterVehicles, EQueryEntitiesFlags.ALL);
		
		return (m_ValidVehicle);
 	}
	
	//------------------------------------------------------------------------------------------------
	private bool ContinueSearch(IEntity ent)
	{
		//Check for inventory component
		m_InventoryManager = SCR_VehicleInventoryStorageManagerComponent.Cast(ent.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		if (m_InventoryManager)
		{
			m_ValidVehicle = ent;
			return false; //stop search
		}
		
		return true; //continue search
	}
	
	//------------------------------------------------------------------------------------------------
	bool FilterVehicles(IEntity ent) 
	{
		return (ent.Type() == Vehicle);
	}

}
