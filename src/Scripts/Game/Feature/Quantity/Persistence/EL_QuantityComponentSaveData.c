[EL_ComponentSaveDataType(EL_QuantityComponentSaveDataClass, EL_QuantityComponent), BaseContainerProps()]
class EL_QuantityComponentSaveDataClass : EL_ComponentSaveDataClass
{
}

[EL_DbName(EL_QuantityComponentSaveData, "Quantity")]
class EL_QuantityComponentSaveData : EL_ComponentSaveData
{
	int m_iQuantity;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		m_iQuantity = EL_QuantityComponent.Cast(worldEntityComponent).GetQuantity();
		if (m_iQuantity == 1) return EL_EReadResult.DEFAULT;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		EL_QuantityComponent quantity = EL_QuantityComponent.Cast(worldEntityComponent);
		quantity.SetQuantity(m_iQuantity);

		// Keep seperate for current frame to have (relevant during load into storages situation). Remove intend on next frame
		quantity.SetTransferIntent(quantity.GetOwner(), true);
		GetGame().GetCallqueue().Call(quantity.RemoveTransferIntent, quantity.GetOwner());
		return true;
	}
}
