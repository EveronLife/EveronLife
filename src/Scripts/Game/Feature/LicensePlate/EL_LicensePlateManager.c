class EL_LicensePlateManagerClass: GenericEntityClass
{
}

class EL_LicensePlateManager: GenericEntity
{
	[Attribute(uiwidget: UIWidgets.Auto)]
	ref EL_LicensePlateGeneratorBase m_GeneratorBase;
	
	static EL_LicensePlateManager s_Instance;
	
	void EL_LicensePlateManager(IEntitySource src, IEntity parent)
	{
		s_Instance = this;
	}
}
