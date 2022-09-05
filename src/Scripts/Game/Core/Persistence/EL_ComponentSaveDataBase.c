[BaseContainerProps()]
class EL_ComponentSaveDataBase
{
	int m_iDataLayoutVersion = 1;

	//------------------------------------------------------------------------------------------------
	array<typename> Requires(); // TODO: Implement automatic entity source changes on prefab edit.

	//------------------------------------------------------------------------------------------------
	//array<typename> CannotCombine(); // TODO: Implement

	//------------------------------------------------------------------------------------------------
	//! Reads the save-data from the world entity component
	//! \param worldEntityComponent the component to read the save-data from
	//! \return true if save-data could be read, false if something failed.
	bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		return EL_DbEntityUtils.StructAutoCopy(worldEntityComponent, this);
	}

	//------------------------------------------------------------------------------------------------
	//! Compares the save-data against a world entity component to find out which save-data belongs to with component in case there are multiple instances of the component present (e.g. storages).
	//! \param worldEntityComponent the component to compare against
	//! \return true if the component is the one the save-data was meant for, false otherwise.
	bool IsFor(notnull GenericComponent worldEntityComponent)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Applies the save-data to the world entity component
	//! \param worldEntityComponent the component to apply the save-data to
	//! \return true if save-data could be applied, false if something failed.
	bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		return EL_DbEntityUtils.StructAutoCopy(this, worldEntityComponent);
	}
}

class EL_ComponentSaveDataType
{
	static ref map<typename, typename> s_mMapping;

	//------------------------------------------------------------------------------------------------
	static typename Get(typename saveDataType)
	{
		if (!s_mMapping) return typename.Empty;

		return s_mMapping.Get(saveDataType);
	}

	//------------------------------------------------------------------------------------------------
	void EL_ComponentSaveDataType(typename saveDataType, typename componentType, string dbEntityName = "")
	{
		if (!s_mMapping) s_mMapping = new map<typename, typename>();

		if (!saveDataType.IsInherited(EL_ComponentSaveDataBase))
		{
			Debug.Error(string.Format("Failed to register '%1' as persistence save-data type for '%2'. '%1' must inherit from '%3'.", saveDataType, componentType, EL_ComponentSaveDataBase));
		}

		s_mMapping.Set(saveDataType, componentType);

		if (dbEntityName)
		{
			EL_DbName.Set(saveDataType, dbEntityName);
		}
	}
}
