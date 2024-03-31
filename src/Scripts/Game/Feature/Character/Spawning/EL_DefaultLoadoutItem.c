[BaseContainerProps()]
class EL_DefaultLoadoutItem
{
	[Attribute()]
	ResourceName m_rPrefab;

	[Attribute(defvalue: "1")]
	int m_iAmount;

	[Attribute(defvalue: EStoragePurpose.PURPOSE_DEPOSIT.ToString(), uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EStoragePurpose))]
	EStoragePurpose m_ePurpose;

	[Attribute()]
	ref array<ref EL_DefaultLoadoutItemComponent> m_aComponentDefaults;

	[Attribute()]
	ref array<ref EL_DefaultLoadoutItem> m_aStoredItems;
}

[BaseContainerProps()]
class EL_DefaultLoadoutItemComponent
{
	void ApplyTo(IEntity item);
}
