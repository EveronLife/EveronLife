[EL_DbName(EL_GarageSaveData, "Garage"), BaseContainerProps()]
class EL_GarageSaveData : EL_EntitySaveDataBase
{
	protected ref map<string, ref array<string>> m_mSavedVehicles = new ref map<string, ref array<string>>;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull IEntity worldEntity)
	{
		//Meta data
		EL_PersistenceComponent persistenceComponent = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		SetId(persistenceComponent.GetPersistentId());
		m_iLastSaved = persistenceComponent.GetLastSaved();

		//Actual Data
		EL_GarageManagerComponent garage = EL_GarageManagerComponent.Cast(worldEntity.FindComponent(EL_GarageManagerComponent));
		m_mSavedVehicles = garage.GetAllVehicles();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull IEntity worldEntity)
	{
		EL_GarageManagerComponent garage = EL_GarageManagerComponent.Cast(worldEntity.FindComponent(EL_GarageManagerComponent));
		garage.SetVehicles(m_mSavedVehicles);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		//Meta data
		saveContext.WriteValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		saveContext.WriteValue("m_sId", GetId());
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);

		//Actual Data
		saveContext.WriteValue("m_mSavedVehicles", m_mSavedVehicles);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		//Meta data
		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		string id;
		loadContext.ReadValue("m_sId", id);
		SetId(id);
		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);

		//Actual Data
		loadContext.ReadValue("m_mSavedVehicles", m_mSavedVehicles);

		return true;
	}
}
