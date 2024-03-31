[BaseContainerProps()]
class EL_DefaultLoadoutItemQuantityComponent : EL_DefaultLoadoutItemComponent
{
	[Attribute("1")]
	int m_iQuantity;

	override void ApplyTo(IEntity item)
	{
		EL_Component<EL_QuantityComponent>.Find(item).SetQuantity(m_iQuantity);
	}
}
