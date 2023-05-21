[EL_ComponentSaveDataType(SCR_CharacterInventoryStorageComponent), BaseContainerProps()]
class EL_CharacterInventoryStorageComponentSaveDataClass : EL_BaseInventoryStorageComponentSaveDataClass
{
	[Attribute(defvalue: "30", uiwidget: UIWidgets.Slider, desc: "Maximum time until the quickbar is synced after a change in SECONDS. Higher values reduce traffic.", params: "1 1000 1")]
	int m_iMaxQuickbarSaveTime;
};

[EL_DbName.Automatic()]
class EL_CharacterInventoryStorageComponentSaveData : EL_BaseInventoryStorageComponentSaveData
{
	ref array<ref EL_PersistentQuickSlotItem> m_aQuickSlotEntities;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		EL_EReadResult result = super.ReadFrom(owner, component, attributes);
		if (!result)
			return EL_EReadResult.ERROR;

		m_aQuickSlotEntities = {};

		SCR_CharacterInventoryStorageComponent inventoryStorage = SCR_CharacterInventoryStorageComponent.Cast(component);
		foreach (int idx, IEntity item : inventoryStorage.GetQuickSlotItems())
		{
			string persistentId = EL_PersistenceComponent.GetPersistentId(item);
			if (!persistentId) continue;

			EL_PersistentQuickSlotItem slot();
			slot.m_iIndex = idx;
			slot.m_sEntityId = persistentId;
			m_aQuickSlotEntities.Insert(slot);
		}

		if (result == EL_EReadResult.DEFAULT && m_aQuickSlotEntities.IsEmpty())
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//! >>> "ApplyTo" happens in modded SCR_RespawnComponent as it needs to be done on the client and data is sent via RPC. <<<
};

class EL_PersistentQuickSlotItem
{
	int m_iIndex;
	string m_sEntityId;
};
