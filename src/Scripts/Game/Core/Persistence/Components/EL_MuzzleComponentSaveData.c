[EL_ComponentSaveDataType(MuzzleComponent), BaseContainerProps()]
class EL_MuzzleComponentSaveDataClass : EL_BaseMuzzleComponentSaveDataClass
{
};

[EL_DbName("Muzzle")]
class EL_MuzzleComponentSaveData : EL_BaseMuzzleComponentSaveData
{
	//------------------------------------------------------------------------------------------------
	override protected bool IsDefaultChambered(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		EL_PersistenceComponent persistence = EL_Component<EL_PersistenceComponent>.Find(owner);
		return MuzzleComponent.Cast(component).GetDefaultMagazinePrefab() &&
			EL_BitFlags.CheckFlags(persistence.GetFlags(), EL_EPersistenceFlags.WAS_SELECTED);
	}
};
