class EL_ComponentFinder<Class T>
{
	//------------------------------------------------------------------------------------------------
	//! Finds the component of the given template type
	//! \param entity World entity to find the component on
	//! \return strong typed component or null if not found
	static T Find(IEntity entity)
	{
		if (!entity) return null;
		return T.Cast(entity.FindComponent(T));
	}
}
