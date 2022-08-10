[EL_ComponentSaveDataType(EL_BaseInventoryStorageComponentSaveData, BaseInventoryStorageComponent, "InventoryStorage"), BaseContainerProps()]
class EL_BaseInventoryStorageComponentSaveData : EL_ComponentSaveDataBase
{
	int m_iPriority;
	EStoragePurpose m_ePurposeFlags;
	ref array<ref EL_PersistentInventoryStorageSlot> m_aSlots;
	
	override bool ReadFrom(GenericComponent worldEntityComponent)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);
		
		//PrintFormat("Now processing %1 on %2", storageComponent, EL_Utils.GetPrefabName(storageComponent.GetOwner()));
		
		m_iPriority = storageComponent.GetPriority();
		m_ePurposeFlags = storageComponent.GetPurpose();
		
		m_aSlots = new array<ref EL_PersistentInventoryStorageSlot>();
		
		//int slots = storageComponent.GetSlotsCount();
		//Print(slots);
		for(int nSlot = 0, slots = storageComponent.GetSlotsCount(); nSlot < slots; nSlot++)
		{
			IEntity slotEntity = storageComponent.Get(nSlot);
			if(!slotEntity)
			{
				//PrintFormat(storageComponent.ToString() + " -> Skipped slot %1 - No entity attached.", nSlot);
				continue;
			}
			
			EL_PersistenceComponent slotPersistenceComponent = EL_PersistenceComponent.Cast(slotEntity.FindComponent(EL_PersistenceComponent));
			if(!slotPersistenceComponent) 
			{
				//PrintFormat(storageComponent.ToString() + " -> Skipped slot %1 - No EL_PersistenceComponent.", nSlot);
				continue;
			}
			
			EL_EntitySaveDataBase saveData = slotPersistenceComponent.Save();
			if(!saveData)
			{
				//PrintFormat(storageComponent.ToString() + " -> Skipped slot %1 - No saveData.", nSlot);
				continue;
			}
			
			// Remove transformation data, as that won't be needed for stored entites
			saveData.m_aComponentsSaveData.Remove(EL_TransformationSaveData);
			
			EL_PersistentInventoryStorageSlot slotInfo();
			slotInfo.m_iSlotId = nSlot;
			slotInfo.m_pEntity = saveData;
			m_aSlots.Insert(slotInfo)
		}
		
		//PrintFormat("Finished processing %1 on %2", storageComponent, EL_Utils.GetPrefabName(storageComponent.GetOwner()));
		
		return true;
	}
	
	override bool IsFor(GenericComponent worldEntityComponent)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);
		
		return (storageComponent.GetPriority() == m_iPriority) && (storageComponent.GetPurpose() == m_ePurposeFlags);
	}
	
	override bool ApplyTo(GenericComponent worldEntityComponent)
	{
		BaseInventoryStorageComponent storageComponent = BaseInventoryStorageComponent.Cast(worldEntityComponent);
		InventoryStorageManagerComponent storageManager = null; //InventoryStorageManagerComponent.Cast(storageComponent.GetOwner().FindComponent(InventoryStorageManagerComponent));
		if(!storageManager) storageManager = EL_GlobalInventoryStorageManagerComponent.GetInstance();
		
		foreach(EL_PersistentInventoryStorageSlot slot : m_aSlots)
		{
			Print(slot.m_pEntity.m_Prefab);
			
			IEntity slotEntity = slot.m_pEntity.Spawn();
			if(!slotEntity)
			{
				PrintFormat("Failed to spawn storage slot entity prefab '%1'.", slot.m_pEntity.m_Prefab);
				continue;
			}
			
			/*
			vector transform[4];
			storageComponent.GetOwner().GetWorldTransform(transform);
			
			BaseGameEntity baseGameEntity = BaseGameEntity.Cast(slotEntity);
			if (baseGameEntity)
			{
				baseGameEntity.Teleport(transform);
			}
			else
			{
				slotEntity.SetWorldTransform(transform);
			}
			
			Print(transform);
			*/
			
			DebugSpawnCallback cb();
			//Print(cb);
			
			Print(slotEntity);
			Print(storageComponent);
			Print(slot.m_iSlotId);
			
			//bool canInsertItemInStorage = storageManager.CanInsertItemInStorage(slotEntity, storageComponent, slot.m_iSlotId);
			//Print(canInsertItemInStorage);
			
			bool tryInsertItemInStorage = storageManager.TryInsertItemInStorage(slotEntity, storageComponent, slot.m_iSlotId, cb);
			Print(tryInsertItemInStorage);
			
			//bool tryMoveItemToStorage = storageManager.TryMoveItemToStorage(slotEntity, storageComponent, slot.m_iSlotId, cb);
			//Print(tryMoveItemToStorage);
			
			if(!tryInsertItemInStorage)
			{
				PrintFormat("Failed to insert storage slot entity prefab '%1' into slot with id '%2'.", slot.m_pEntity.m_Prefab, slot.m_iSlotId);
				SCR_EntityHelper.DeleteEntityAndChildren(slotEntity);
			}
			
		}
		
		return true;
	}
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("dataLayoutVersion", 1);
		saveContext.WriteValue("m_iPriority", m_iPriority);
		saveContext.WriteValue("m_ePurposeFlags", m_ePurposeFlags);
		saveContext.WriteValue("m_aSlots", m_aSlots);
		
		return true;
	}
	
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;
		
		int dataLayoutVersion;
		loadContext.ReadValue("dataLayoutVersion", dataLayoutVersion);
		loadContext.ReadValue("m_iPriority", m_iPriority);
		loadContext.ReadValue("m_ePurposeFlags", m_ePurposeFlags);
		loadContext.ReadValue("m_aSlots", m_aSlots);
		
		return true;
	}
}

class EL_PersistentInventoryStorageSlot
{
	int m_iSlotId;
	ref EL_EntitySaveDataBase m_pEntity;
}

class DebugSpawnCallback : ScriptedInventoryOperationCallback
{
	override void OnComplete()
	{
		Print("OnComplete");
	}

	override void OnFailed()
	{
		Print("OnComplete");
	}
}
