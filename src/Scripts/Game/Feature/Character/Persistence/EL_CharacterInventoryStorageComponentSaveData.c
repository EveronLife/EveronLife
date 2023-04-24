[EL_ComponentSaveDataType(EL_CharacterInventoryStorageComponentSaveDataClass, SCR_CharacterInventoryStorageComponent), BaseContainerProps()]
class EL_CharacterInventoryStorageComponentSaveDataClass : EL_BaseInventoryStorageComponentSaveDataClass
{
	[Attribute(defvalue: "30", uiwidget: UIWidgets.Slider, desc: "Maximum time until the quickbar is synced after a change in SECONDS. Higher values reduce traffic.", params: "1 1000 1")]
	int m_iMaxQuickbarSaveTime;
}

[EL_DbName(EL_CharacterInventoryStorageComponentSaveData, "CharacterInventoryStorage")]
class EL_CharacterInventoryStorageComponentSaveData : EL_BaseInventoryStorageComponentSaveData
{
	ref array<ref EL_PersistentQuickSlotItem> m_aQuickSlotEntities;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		if (!super.ReadFrom(worldEntityComponent, attributes)) return EL_EReadResult.ERROR;

		SCR_CharacterInventoryStorageComponent inventoryStorage = SCR_CharacterInventoryStorageComponent.Cast(worldEntityComponent);

		m_aQuickSlotEntities = new array<ref EL_PersistentQuickSlotItem>();

		foreach (int idx, IEntity item : inventoryStorage.GetQuickSlotItems())
		{
			string persistentId = EL_PersistenceComponent.GetPersistentId(item);
			if (!persistentId) continue;

			EL_PersistentQuickSlotItem slot();
			slot.m_iIndex = idx;
			slot.m_sEntityId = persistentId;
			m_aQuickSlotEntities.Insert(slot);
		}

		return EL_EReadResult.OK;
	}

	//! >>> "ApplyTo" happens in modded SCR_RespawnComponent as it needs to be done on the client and data is sent via RPC. <<<
}

class EL_PersistentQuickSlotItem
{
	int m_iIndex;
	string m_sEntityId;
}
