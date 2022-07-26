class EL_PersistentCharacter : EL_PersistentEntity
{
	string m_AccountId;
	
	/*
		m_Components {
			EL_PersistentTransformation,
			EL_PersistentRoleplayIdentity,
			EL_PersistentCharacterStats,
			EL_PersistentStorage,
		}
	*/
}

[EL_DbEntityRepositoryType(EL_PersistentCharacterRepository, EL_PersistentCharacter)]
class EL_PersistentCharacterRepository : EL_DbEntityRepository<EL_PersistentCharacter>
{
	// TODO find by account id
	
	// TODO find by full/lastname
}
