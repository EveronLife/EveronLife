class EL_SpawnUtils
{
	//------------------------------------------------------------------------------------------------
	static IEntity FindFreeSpawnPoint(IEntity parent)
	{
		if (!parent)
			return null;

		IEntity child = parent.GetChildren();
		while (child)
		{
			EL_VehicleSpawnPoint vehicleSpawnPoint = EL_VehicleSpawnPoint.Cast(child.FindComponent(EL_VehicleSpawnPoint));
			if (vehicleSpawnPoint && vehicleSpawnPoint.IsFree())
			{
				return child;
			}
			child = child.GetSibling();
		}
		return null;
	}

	//------------------------------------------------------------------------------------------------
	static IEntity FindSpawnPoint(IEntity parent)
	{
		if (!parent)
			return null;

		IEntity child = parent.GetChildren();
		while (child)
		{
			EL_VehicleSpawnPoint vehicleSpawnPoint = EL_VehicleSpawnPoint.Cast(child.FindComponent(EL_VehicleSpawnPoint));
			if (vehicleSpawnPoint)
				return child;
			child = child.GetSibling();
		}
		return null;
	}
}
