[EL_ComponentSaveDataType(EL_QuantityComponentSaveData, EL_QuantityComponent, "Quantity"), BaseContainerProps()]
class EL_QuantityComponentSaveData : EL_ComponentSaveDataBase
{
	int m_iQuantity;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		m_iQuantity = EL_QuantityComponent.Cast(worldEntityComponent).GetQuantity();
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		EL_QuantityComponent quantity = EL_QuantityComponent.Cast(worldEntityComponent);
		quantity.SetQuantity(m_iQuantity);

		// Keep seperate for current frame to have (relevant during load into storages situation). Remove intend on next frame
		quantity.SetTransferIntent(quantity.GetOwner(), true);
		GetGame().GetCallqueue().Call(quantity.RemoveTransferIntent, quantity.GetOwner());
		return true;
	}
}
