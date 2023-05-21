[BaseContainerProps()]
class EL_ComponentSaveDataClass
{
	[Attribute("1", desc: "Should this component save-data type be saved/loaded or if disabled skipped.")]
	bool m_bEnabled;

	bool m_bTrimDefaults; //"Inherited" attribute value from parent save data

	//------------------------------------------------------------------------------------------------
	array<typename> Requires(); // TODO: Implement error if not satisfied

	//------------------------------------------------------------------------------------------------
	array<typename> CannotCombine(); // TODO: Implement error if not satisfied
};

class EL_ComponentSaveData
{
	int m_iDataLayoutVersion = 1;

	//------------------------------------------------------------------------------------------------
	//! Reads the save-data from the world entity component
	//! \param owner of the component
	//! \param component to read the save-data from
	//! \param attributes the class-class shared configuration attributes assigned in the world editor
	//! \return EL_EReadResult.OK if save-data could be read, ERROR if something failed, DEFAULT if the data could be trimmed
	EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		return EL_DbEntityUtils.StructAutoCopy(component, this);
	}

	//------------------------------------------------------------------------------------------------
	//! Compares the save-data against a world entity component to find out which save-data belongs to with component in case there are multiple instances of the component present (e.g. storages).
	//! \param owner of the component
	//! \param component to compare against
	//! \param attributes the class-class shared configuration attributes assigned in the world editor
	//! \return true if the component is the one the save-data was meant for, false otherwise.
	bool IsFor(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Applies the save-data to the world entity component
	//! \param owner of the component
	//! \param component to apply the save-data to
	//! \param attributes the class-class shared configuration attributes assigned in the world editor
	//! \return true if save-data could be applied, false if something failed.
	EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		return EL_DbEntityUtils.StructAutoCopy(this, component);
	}

	//------------------------------------------------------------------------------------------------
	//! Compare component save-data instances to see if there is any noteable difference
	//! \param other component save-data to compare against
	//! \return true if save-data is considered to describe the same data. False on differences.
	bool Equals(notnull EL_ComponentSaveData other)
	{
		return false;
	}
};

class EL_ComponentSaveDataType : BaseContainerCustomTitle
{
	static ref map<typename, typename> s_mMapping;

	#ifdef WORKBENCH
	protected string m_sWorkbenchTitle;
	#endif

	//------------------------------------------------------------------------------------------------
	static typename Get(typename saveDataType)
	{
		if (!s_mMapping) return typename.Empty;

		return s_mMapping.Get(saveDataType);
	}

	//------------------------------------------------------------------------------------------------
	void EL_ComponentSaveDataType(typename componentType)
	{
		typename saveDataType = EL_ReflectionUtils.GetAttributeParent();

		if (!saveDataType.IsInherited(EL_ComponentSaveDataClass))
		{
			Debug.Error(string.Format("Failed to register '%1' as persistence save-data type for '%2'. '%1' must inherit from '%3'.", saveDataType, componentType, EL_ComponentSaveDataClass));
		}

		if (!saveDataType.ToString().EndsWith("Class"))
		{
			Debug.Error(string.Format("Failed to register '%1' as persistence save-data type for '%2'. '%1' must follow xyzClass naming convention.", saveDataType));
		}

		if (!s_mMapping)
			s_mMapping = new map<typename, typename>();

		s_mMapping.Set(saveDataType, componentType);

		#ifdef WORKBENCH
		m_sWorkbenchTitle = componentType.ToString();
		#endif
	}

	#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		title = m_sWorkbenchTitle;
		return true;
	}
	#endif
};
