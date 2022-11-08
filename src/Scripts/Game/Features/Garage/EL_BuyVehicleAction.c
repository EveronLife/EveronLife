class EL_BuyVehicleAction : ScriptedUserAction
{	
	//------------------------------------------------------------------------------------------------
	void SpawnVehicle(IEntity userEntity, ResourceName prefabToSpawn, Color color)
	{
		IEntity freeSpawnPoint = EL_SpawnUtils.FindFreeSpawnPoint(SCR_EntityHelper.GetMainParent(GetOwner()));
		IEntity newCar = EL_Utils.SpawnEntityPrefab(prefabToSpawn, freeSpawnPoint.GetOrigin(), freeSpawnPoint.GetYawPitchRoll());
		
		/*ParametricMaterialInstanceComponent materialOverride = ParametricMaterialInstanceComponent.Cast(newCar.FindComponent(ParametricMaterialInstanceComponent));
		if (!materialOverride)
		{
			Print("[EL-Shop] Vehicle has no ParametricMaterialInstanceComponent!", LogLevel.ERROR);
			return;
		}
		materialOverride.SetColor(ARGB(color.A() * COLOR_1_TO_255, color.R() * COLOR_1_TO_255, color.G() * COLOR_1_TO_255, color.B() * COLOR_1_TO_255));
		*/
		EL_CharacterOwnerComponent charOwnerComp = EL_CharacterOwnerComponent.Cast(newCar.FindComponent(EL_CharacterOwnerComponent));
		
		//Account ID for now. Later characterID
		string vehOwnerId = EL_Utils.GetPlayerUID(userEntity);
		charOwnerComp.SetCharacterOwner(vehOwnerId);
		Print("User with ID: " + vehOwnerId + " bought vehicle: " + newCar);
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SpawnVehicle(pUserEntity, SCR_BaseContainerTools.GetPrefabResourceName(GetOwner().GetPrefabData().GetPrefab()), Color.FromInt(COLOR_BLUE));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Buy Vehicle");
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		//Check if spawnpoint is empty..
		if (!EL_SpawnUtils.FindFreeSpawnPoint(SCR_EntityHelper.GetMainParent(GetOwner())))
		{
			SetCannotPerformReason("No free spawn points");
			return false;
		}
		return true;
	}
	
	//------------------------------------------------------------------------------------------------	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
	}

}
