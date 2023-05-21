[EL_ComponentSaveDataType(EL_QuantityComponent), BaseContainerProps()]
class EL_QuantityComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName.Automatic()]
class EL_QuantityComponentSaveData : EL_ComponentSaveData
{
	int m_iQuantity;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		m_iQuantity = EL_QuantityComponent.Cast(component).GetQuantity();
		if (m_iQuantity == 1) return EL_EReadResult.DEFAULT;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		EL_QuantityComponent quantity = EL_QuantityComponent.Cast(component);
		quantity.SetQuantity(m_iQuantity);

		// Keep seperate for current frame to have (relevant during load into storages situation). Remove intend on next frame
		quantity.SetTransferIntent(quantity.GetOwner(), true);
		GetGame().GetCallqueue().Call(quantity.RemoveTransferIntent, quantity.GetOwner());
		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_QuantityComponentSaveData otherData = EL_QuantityComponentSaveData.Cast(other);
		return m_iQuantity == otherData.m_iQuantity;
	}
};
