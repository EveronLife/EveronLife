class EL_VehicleInventoryStorageAccessControlClass: EL_InventoryStorageAccessControlClass
{
};


class EL_VehicleInventoryStorageAccessControl : EL_InventoryStorageAccessControl
{
	
	
	override bool IsLocked(IEntity entity) {
		
		
		//Get the vehicles lock
		auto vehicleLock = EL_VehicleLockComponent.Cast(this.GetOwner().FindComponent(EL_VehicleLockComponent));
		
		//Check if there is a vehicle lock
		if (vehicleLock) {
			
			//Check if the user can access the vehicles inventory
			return vehicleLock.IsLocked(entity, null);
		
		}
		
		
		return false;
	}
}