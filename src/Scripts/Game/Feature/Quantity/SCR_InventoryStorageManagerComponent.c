modded class SCR_InventoryStorageManagerComponent
{
	protected static ref map<IEntity, bool> m_mELQuantityTransferIntents;

	//------------------------------------------------------------------------------------------------
	void EL_RequestQuantityTransfer(notnull IEntity sourceEntity, notnull IEntity destinationEntity)
	{
		Rpc(RPC_EL_QuantityTransfer, EL_Utils.GetRplId(sourceEntity), EL_Utils.GetRplId(destinationEntity));
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_QuantityTransfer(RplId quantitySourceRplId, RplId quantityDestinationRplId)
	{
		EL_QuantityComponent quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(EL_Utils.FindEntityByRplId(quantitySourceRplId));
		if (!quantitySource || !EL_CanManipulate(quantitySource.GetOwner())) return;

		EL_QuantityComponent quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(EL_Utils.FindEntityByRplId(quantityDestinationRplId));
		if (!quantityDestination || !EL_CanManipulate(quantityDestination.GetOwner())) return;

		quantityDestination.Combine(quantitySource);
	}

	//------------------------------------------------------------------------------------------------
	void EL_SetTransferIntent(notnull IEntity sourceEntity, bool keepSeperate)
	{
		Rpc(RPC_EL_SetTransferIntent, EL_Utils.GetRplId(sourceEntity), keepSeperate);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_SetTransferIntent(RplId sourceRplId, bool keepSeperate)
	{
		IEntity sourceEntity = EL_Utils.FindEntityByRplId(sourceRplId);
		if (!sourceEntity || !EL_CanManipulate(sourceEntity)) return;

		if (!m_mELQuantityTransferIntents) m_mELQuantityTransferIntents = new map<IEntity, bool>();
		m_mELQuantityTransferIntents.Set(sourceEntity, keepSeperate);
	}

	//------------------------------------------------------------------------------------------------
	protected bool EL_CanManipulate(IEntity entity)
	{
		const int maxManipulationRange = 10;
		return (vector.Distance(entity.GetOrigin(), GetOwner().GetOrigin()) < maxManipulationRange);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		EL_QuantityComponent quantitySource;
		if (Replication.IsServer()) quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(item);
		if (quantitySource)
		{
			bool keepSeperate;
			bool intentSet;
			if (m_mELQuantityTransferIntents)
			{
				intentSet = m_mELQuantityTransferIntents.Find(item, keepSeperate);
				if (intentSet) m_mELQuantityTransferIntents.Remove(item);
			}

			if (!keepSeperate)
			{
				BaseInventoryStorageComponent storageRestriction;
				if (intentSet) storageRestriction = storageOwner;

				//If Quantity source fully distributed onto other items, abort. Quantity sync on the item will cause menu refresh, so no need to call it here
				if (EL_TransferQuantity(quantitySource, storageRestriction)) return;
			}
		}

		super.OnItemAdded(storageOwner, item);
	}

	//------------------------------------------------------------------------------------------------
	protected bool EL_TransferQuantity(EL_QuantityComponent quantitySource, BaseInventoryStorageComponent storageRestriction = null)
	{
		array<IEntity> combineableItems();
		EL_QuantityCombineablePredicate quantityCombinePredicate(quantitySource, storageRestriction);
		FindItems(combineableItems, quantityCombinePredicate);

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
}

class EL_QuantityCombineablePredicate: InventorySearchPredicate
{
	protected EL_QuantityComponent m_pQuantitySource;
	protected BaseInventoryStorageComponent m_pStorageRestriction;

	void EL_QuantityCombineablePredicate(notnull EL_QuantityComponent quantitySource, BaseInventoryStorageComponent storageRestriction = null)
	{
		m_pQuantitySource = quantitySource;
		m_pStorageRestriction = storageRestriction;
		QueryComponentTypes.Insert(EL_QuantityComponent);
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return (!m_pStorageRestriction || m_pStorageRestriction == storage) && (EL_QuantityComponent.Cast(queriedComponents[0])).CanCombine(m_pQuantitySource);
	}
}
