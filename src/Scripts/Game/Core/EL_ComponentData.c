class EL_ComponentData<Class T>
{
	//------------------------------------------------------------------------------------------------
	//! Gets the shared component data class instance
	//! \param entity World entity that holds the component to get the data class for
	//! \return strong typed component data class or null if not found
	static T Get(IEntity entity)
	{
		if (!entity) return null;
		typename dataClassType = T;
		string componentTypeString = dataClassType.ToString();
		typename componentType = componentTypeString.Substring(0, componentTypeString.Length() - 5).ToType();
		return Get(ScriptComponent.Cast(entity.FindComponent(componentType)));
	}

	//------------------------------------------------------------------------------------------------
	//! Gets the shared component data class instance
	//! \param component The script component instance to get the data class for.
	//! \return strong typed component data class or null if not found
	static T Get(ScriptComponent component)
	{
		if (!component) return null;
		return T.Cast(component.GetComponentData(component.GetOwner()));
	}
};
