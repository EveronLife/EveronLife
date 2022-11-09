class EL_QuantityComponentClass : ScriptComponentClass
{
	[Attribute(defvalue: "0", desc: "Inclusive maximal quantity this item can stack to. 0 means unlimited stack size.", params: "0 2000000000 1")]
	int m_iMaxQuantity;

	//------------------------------------------------------------------------------------------------
	override static array<typename> CannotCombine(IEntityComponentSource src)
	{
		return {EL_QuantityComponent}; // Prevent user from adding multiple quantity components
	}
}

class EL_QuantityComponent : ScriptComponent
{
	protected static ref map<IEntity, bool> s_mQuantityTransferIntents;

	[RplProp(onRplName: "OnQuantityChanged")]
	protected int m_iQuantity = 1;

	//------------------------------------------------------------------------------------------------
	int GetMaxQuantity()
	{
		int result = EL_QuantityComponentClass.Cast(GetComponentData(GetOwner())).m_iMaxQuantity;
		if (result == 0) result = int.MAX;
		return result;
	}

	//------------------------------------------------------------------------------------------------
	int GetRemainingCapacity()
	{
		return GetMaxQuantity() - m_iQuantity;
	}

	//------------------------------------------------------------------------------------------------
	int GetQuantity()
	{
		return m_iQuantity;
	}

	//------------------------------------------------------------------------------------------------
	void AddQuantity(int quantity)
	{
		SetQuantity(m_iQuantity + quantity);
	}

	//------------------------------------------------------------------------------------------------
	void SetQuantity(int quantity)
	{
		if (!EL_ComponentFinder<RplComponent>.Find(GetOwner()).IsMaster()) return;
		m_iQuantity = EL_Utils.MinInt(quantity, GetMaxQuantity());
		Replication.BumpMe();
		OnQuantityChanged(); // Call on authority
	}

	//------------------------------------------------------------------------------------------------
	protected void OnQuantityChanged()
	{
		RefreshInventory(GetOwner());
	}

	//------------------------------------------------------------------------------------------------
	bool CanCombine(notnull EL_QuantityComponent quantitySource)
	{
		bool notSelf = this != quantitySource;
		bool prefabEqual = EL_Utils.GetPrefabName(GetOwner()) == EL_Utils.GetPrefabName(quantitySource.GetOwner());
		bool capacityLeft = m_iQuantity < GetMaxQuantity();
		return notSelf && prefabEqual && capacityLeft;
	}

	//------------------------------------------------------------------------------------------------
	//! Return the quantity left after combine
	int Combine(notnull EL_QuantityComponent quantitySource, int amount = -1, bool setQuantity = false)
	{
		if (!CanCombine(quantitySource)) return quantitySource.GetQuantity();

		if (amount == -1) amount = quantitySource.GetQuantity();

		int transferQuantity = EL_Utils.MinInt(GetRemainingCapacity(), amount);

		if (setQuantity)
		{
			SetQuantity(transferQuantity);
		}
		else
		{
			AddQuantity(transferQuantity);
		}

		if (quantitySource.GetQuantity() > transferQuantity)
		{
			// Reduce quantity of source
			quantitySource.AddQuantity(-transferQuantity);
		}
		else
		{
			// Source transferred entire quantity, so delete it
			SCR_EntityHelper.DeleteEntityAndChildren(quantitySource.GetOwner());
			return 0;
		}

		return quantitySource.GetQuantity();
	}

	//------------------------------------------------------------------------------------------------
	void Split(int splitSize = -1)
	{
		if (splitSize == -1) splitSize = m_iQuantity / 2;

		IEntity owner = GetOwner();
		IEntity destinationEntity = EL_Utils.SpawnEntityPrefab(EL_Utils.GetPrefabName(owner), owner.GetOrigin());
		EL_QuantityComponent quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(destinationEntity);
		if (!quantityDestination) return;

		quantityDestination.Combine(this, splitSize, true);

		SetTransferIntent(destinationEntity, true);

		InventoryItemComponent sourceInventoryItem = EL_ComponentFinder<InventoryItemComponent>.Find(owner);

		// Ground item, just move it somewhere else so it can be picked up seperatly from the source stack
		if (!sourceInventoryItem.GetParentSlot())
		{
			vector maxDims;
			owner.GetBounds(null, maxDims);
			float minRadius = Math.Max(maxDims[0], maxDims[2]); //Bounding radius
			RandomGenerator random();
			destinationEntity.SetOrigin(random.GenerateRandomPointInRadius(minRadius + 0.05, minRadius + 0.25, owner.GetOrigin()));
			return;
		}

		// TODO: Refactor to use responsible storage manager if owned, if not owned always global one for TryInsertItemInStorage. No rpcs needed anymore. (likely 0.9.6 or 0.9.7 patch)
		BaseInventoryStorageComponent storage = sourceInventoryItem.GetParentSlot().GetStorage();
		InventoryStorageManagerComponent storageManager = EL_InventoryUtils.GetResponsibleStorageManager(owner);
		if (!storageManager) storageManager = EL_GlobalInventoryStorageManagerComponent.GetInstance();

		RplComponent replication = EL_ComponentFinder<RplComponent>.Find(storageManager.GetOwner());
		if (replication.IsOwner())
		{
			storageManager.TryInsertItemInStorage(destinationEntity, storage);
		}
		else
		{
			if (storageManager.Type().IsInherited(SCR_InventoryStorageManagerComponent))
			{
				SCR_InventoryStorageManagerComponent.Cast(storageManager).EL_RequestInsertItemInStorage(destinationEntity, storage);
			}
			else if (storageManager.Type().IsInherited(SCR_VehicleInventoryStorageManagerComponent))
			{
				SCR_VehicleInventoryStorageManagerComponent.Cast(storageManager).EL_RequestInsertItemInStorage(destinationEntity, storage);
			}
			else
			{
				Debug.Error(string.Format("Unknown storage manager type: '%1'", storage.Type().ToString()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	static void SetTransferIntent(notnull IEntity sourceEntity, bool keepSeperate)
	{
		if (!s_mQuantityTransferIntents) s_mQuantityTransferIntents = new map<IEntity, bool>();
		s_mQuantityTransferIntents.Set(sourceEntity, keepSeperate);
	}

	//------------------------------------------------------------------------------------------------
	static void RemoveTransferIntent(notnull IEntity sourceEntity)
	{
		if (s_mQuantityTransferIntents) s_mQuantityTransferIntents.Remove(sourceEntity);
	}

	//------------------------------------------------------------------------------------------------
	static bool HandleOnItemAdded(InventoryStorageManagerComponent invManager, BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		EL_QuantityComponent quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(item);
		if (!quantitySource) return false;

		bool ignoreSuper;
		if (Replication.IsServer())
		{
			bool keepSeperate;
			bool intentSet;
			if (s_mQuantityTransferIntents)
			{
				intentSet = s_mQuantityTransferIntents.Find(item, keepSeperate);
				if (intentSet) RemoveTransferIntent(item);
			}

			if (!keepSeperate)
			{
				BaseInventoryStorageComponent storageRestriction;
				if (intentSet) storageRestriction = storageOwner;

				//If Quantity source fully distributed onto other items, abort. Quantity sync on the item will cause menu refresh, so no need to call it here
				if (TransferQuantity(invManager, quantitySource, storageRestriction)) ignoreSuper = true;
			}
		}

		RefreshInventory(item);

		return ignoreSuper;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool TransferQuantity(InventoryStorageManagerComponent invManager, EL_QuantityComponent quantitySource, BaseInventoryStorageComponent storageRestriction = null)
	{
		array<IEntity> combineableItems();
		EL_QuantityCombineablePredicate quantityCombinePredicate(quantitySource, storageRestriction);
		invManager.FindItems(combineableItems, quantityCombinePredicate);

		// Quicky sort all item quantities by PADDED_QUANTITY:INDEX string sort
		array<string> sortKeys();
		map<string, ref array<EL_QuantityComponent>> componentMap();
		foreach (IEntity combineableItem : combineableItems)
		{
			EL_QuantityComponent quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(combineableItem);
			string key = quantityDestination.GetQuantity().ToString(10);

			if (!sortKeys.Contains(key)) sortKeys.Insert(key);

			array<EL_QuantityComponent> sameQuantityComponents = componentMap.Get(key);
			if (!sameQuantityComponents)
			{
				sameQuantityComponents = {};
				componentMap.Set(key, sameQuantityComponents);
			}

			sameQuantityComponents.Insert(quantityDestination);
		}
		sortKeys.Sort(true);

		foreach (string sortKey : sortKeys)
		{
			array<EL_QuantityComponent> sameQuantityComponents = componentMap.Get(sortKey);
			foreach (EL_QuantityComponent quantityDestination : sameQuantityComponents)
			{
				// In case 0 quantity remains on the source it was deleted so the caller scope needs to skip the super call
				if (quantityDestination.Combine(quantitySource) == 0) return true;
			}
		}

		// Some quantity remained on the source so allow caller scope to conttinue to super call
		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static void RefreshInventory(IEntity item)
	{
		SCR_InventoryMenuUI inventoryMenu = SCR_InventoryMenuUI.EL_GetCurrentInstance();
		if (inventoryMenu) inventoryMenu.EL_QuantityRefresh(item);
	}
}

class EL_QuantityCombineablePredicate: InventorySearchPredicate
{
	protected EL_QuantityComponent m_pQuantitySource;
	protected BaseInventoryStorageComponent m_pStorageRestriction;

	//------------------------------------------------------------------------------------------------
	void EL_QuantityCombineablePredicate(notnull EL_QuantityComponent quantitySource, BaseInventoryStorageComponent storageRestriction = null)
	{
		m_pQuantitySource = quantitySource;
		m_pStorageRestriction = storageRestriction;
		QueryComponentTypes.Insert(EL_QuantityComponent);
	}

	//------------------------------------------------------------------------------------------------
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return (!m_pStorageRestriction || m_pStorageRestriction == storage) && (EL_QuantityComponent.Cast(queriedComponents[0])).CanCombine(m_pQuantitySource);
	}
}
