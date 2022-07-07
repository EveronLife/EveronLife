[EL_DbEntityRepositoryType(EL_PersistentVehicle, EL_PersistentVehicleRepository)]
class EL_PersistentVehicle : EL_PersistentEntity
{
	EL_DbEntityId m_OwnerId;
	
	string m_LicensePlate;
}

class EL_PersistentVehicleRepository : EL_DbEntityRepository<EL_PersistentVehicle>
{
	// TODO find by license plate
}
