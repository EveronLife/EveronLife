class EL_IDCardManager: GenericEntity
{
	[Attribute(uiwidget: UIWidgets.Auto)]
	ref EL_IDCardGeneratorBase m_GeneratorBase;
	
	static EL_IDCardManager s_Instance;
	
	void EL_IDCardManager(IEntitySource src, IEntity parent)
	{
		s_Instance = this;
	}
}