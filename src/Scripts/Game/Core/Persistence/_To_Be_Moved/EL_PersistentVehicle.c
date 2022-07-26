class EL_PersistentVehicle : EL_PersistentEntity
{
	string m_OwnerId;
	
	string m_LicensePlate;
}

[EL_DbEntityRepositoryType(EL_PersistentVehicleRepository, EL_PersistentVehicle)]
class EL_PersistentVehicleRepository : EL_DbEntityRepository<EL_PersistentVehicle>
{
	// TODO find by license plate
}
