[EL_DbEntityRepositoryType(EL_VehicleDbEntity, EL_VehicleDbEntityRepository)]
class EL_VehicleDbEntity : EL_DbEntity
{
	EL_DbEntityId m_OwnerId;
	
	string m_LicensePlate;
	
	//todo what to do with components?!?
}

class EL_VehicleDbEntityRepository : EL_DbEntityRepository<EL_VehicleDbEntity>
{
}
