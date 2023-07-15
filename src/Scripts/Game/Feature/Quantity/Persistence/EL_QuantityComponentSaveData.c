[EPF_ComponentSaveDataType(EL_QuantityComponent), BaseContainerProps()]
class EL_QuantityComponentSaveDataClass : EPF_ComponentSaveDataClass
{
};

[EDF_DbName.Automatic()]
class EL_QuantityComponentSaveData : EPF_ComponentSaveData
{
	int m_iQuantity;

	//------------------------------------------------------------------------------------------------
	override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		m_iQuantity = EL_QuantityComponent.Cast(component).GetQuantity();
		
		if (m_iQuantity == 1) 
			return EPF_EReadResult.DEFAULT;
		
		return EPF_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		EL_QuantityComponent quantity = EL_QuantityComponent.Cast(component);
		quantity.SetQuantity(m_iQuantity);

		// Keep seperate for current frame to have (relevant during load into storages situation). Remove intent on next frame
		quantity.SetTransferIntent(quantity.GetOwner(), true);
		GetGame().GetCallqueue().Call(quantity.RemoveTransferIntent, quantity.GetOwner());
		return EPF_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EPF_ComponentSaveData other)
	{
		EL_QuantityComponentSaveData otherData = EL_QuantityComponentSaveData.Cast(other);
		return m_iQuantity == otherData.m_iQuantity;
	}
};
