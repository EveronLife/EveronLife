[ComponentEditorProps(category: "EveronLife/Feature/Quantity", description: "Virtual quantities for inventory items.")]
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

	protected ref ScriptInvoker m_pOnQuantityChanged;

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
	//! Returns new quantity after operation
	int AddQuantity(int quantity, bool allowPartial = true, out int change = 0)
	{
		if (quantity == 0) return 0;

		if (quantity > 0)
		{
			int capacity = GetRemainingCapacity();
			if (quantity > capacity)
			{
				if (!allowPartial) return 0;
				quantity = capacity;
			}
		}
		else if (quantity < -m_iQuantity)
		{
			if (!allowPartial) return 0;
			quantity = -m_iQuantity;
		}

		SetQuantity(m_iQuantity + quantity);
		change = quantity;
		return m_iQuantity;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns new quantity after operation
	int RemoveQuantity(int quantity, bool allowPartial = true, out int change = 0)
	{
		return AddQuantity(-quantity, allowPartial, change);
	}

	//------------------------------------------------------------------------------------------------
	bool SetQuantity(int quantity)
	{
		if (!EL_Component<RplComponent>.Find(GetOwner()).IsMaster()) return false;
		if ((quantity < 0) || (quantity > GetMaxQuantity())) return false;

		m_iQuantity = quantity;

		if (quantity == 0)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(GetOwner());
		}
		else
		{
			Replication.BumpMe();
		}

		OnQuantityChanged(); // Call on authority

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected void OnQuantityChanged()
	{
		RefreshInventory(GetOwner());
		if (m_pOnQuantityChanged) m_pOnQuantityChanged.Invoke(this);
	}

	//------------------------------------------------------------------------------------------------
	ScriptInvoker GetOnQuantityChanged()
	{
		if (!m_pOnQuantityChanged) m_pOnQuantityChanged = new ScriptInvoker();
		return m_pOnQuantityChanged;
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
	//! Return new quantity after operation
	int Combine(notnull EL_QuantityComponent quantitySource, int amount = -1, out int transferred = 0)
	{
		if (CanCombine(quantitySource))
		{
			if (amount == -1) amount = quantitySource.GetQuantity();

			transferred = EL_Utils.MinInt(GetRemainingCapacity(), amount);

			AddQuantity(transferred);
			quantitySource.AddQuantity(-transferred);
		}

		return m_iQuantity;
	}

	//------------------------------------------------------------------------------------------------
	void Split(int splitSize = -1)
	{
		if (splitSize == -1) splitSize = m_iQuantity / 2;

		IEntity owner = GetOwner();
		IEntity destinationEntity = EL_Utils.SpawnEntityPrefab(EL_Utils.GetPrefabName(owner), owner.GetOrigin());
		EL_QuantityComponent quantityDestination = EL_Component<EL_QuantityComponent>.Find(destinationEntity);
		if (!quantityDestination) return;

		AddQuantity(-splitSize);
		quantityDestination.SetQuantity(splitSize);

		SetTransferIntent(destinationEntity, true);

		InventoryItemComponent sourceInventoryItem = EL_Component<InventoryItemComponent>.Find(owner);

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

		BaseInventoryStorageComponent storage = sourceInventoryItem.GetParentSlot().GetStorage();
		InventoryStorageManagerComponent storageManager = EL_InventoryUtils.GetResponsibleStorageManager(owner);
		if (!storageManager) storageManager = EPF_GlobalInventoryStorageManagerComponent.GetInstance();
		storageManager.TryInsertItemInStorage(destinationEntity, storage);
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
		EL_QuantityComponent quantitySource = EL_Component<EL_QuantityComponent>.Find(item);
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
				ignoreSuper = TransferQuantity(invManager, quantitySource, storageRestriction);
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

		array<EL_QuantityComponent> quantityComponents = ExtractQuantityComponents(combineableItems);

		foreach (EL_QuantityComponent quantityDestination : SortByQuantity(quantityComponents))
		{
			// In case 0 quantity remains on the source it was deleted so the caller scope needs to skip the super call
			quantityDestination.Combine(quantitySource);
			if (!quantitySource || quantitySource.GetQuantity() == 0) return true;
		}

		// Some quantity remained on the source so allow caller scope to conttinue to super call
		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! Sort quantity components in descending order
	static array<EL_QuantityComponent> SortByQuantity(notnull array<EL_QuantityComponent> components, bool descending = true)
	{
		array<string> sortKeys();
		map<string, ref array<EL_QuantityComponent>> componentMap();
		foreach (EL_QuantityComponent quantityComponent : components)
		{
			string key = quantityComponent.GetQuantity().ToString(10);

			if (!sortKeys.Contains(key)) sortKeys.Insert(key);

			array<EL_QuantityComponent> sameQuantity = componentMap.Get(key);
			if (!sameQuantity)
			{
				sameQuantity = {};
				componentMap.Set(key, sameQuantity);
			}

			sameQuantity.Insert(quantityComponent);
		}
		sortKeys.Sort(descending);

		array<EL_QuantityComponent> sorted();
		sorted.Reserve(components.Count());
		foreach (string sortKey : sortKeys)
		{
			foreach (EL_QuantityComponent quantityDestination : componentMap.Get(sortKey))
			{
				sorted.Insert(quantityDestination);
			}
		}

		return sorted;
	}

	//------------------------------------------------------------------------------------------------
	static array<EL_QuantityComponent> ExtractQuantityComponents(notnull array<IEntity> entities)
	{
		array<EL_QuantityComponent> components();
		components.Reserve(entities.Count());
		foreach(IEntity entity : entities)
		{
			components.Insert(EL_Component<EL_QuantityComponent>.Find(entity));
		}

		return components;
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
