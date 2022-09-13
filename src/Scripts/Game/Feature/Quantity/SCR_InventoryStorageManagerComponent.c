modded class SCR_InventoryStorageManagerComponent
{
	protected static ref map<IEntity, IEntity> m_mELQuantityTargetIntents;

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
		if (!quantitySource) return;

		EL_QuantityComponent quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(EL_Utils.FindEntityByRplId(quantityDestinationRplId));
		if (!quantityDestination) return;

		// Validate that rpc sender is near the object to be manipulated
		if (!CanManipulate(quantitySource.GetOwner(), quantityDestination.GetOwner())) return;

		quantityDestination.Combine(quantitySource);
	}

	//------------------------------------------------------------------------------------------------
	void EL_SetQuantityTargetIntent(notnull IEntity sourceEntity, notnull IEntity destinationEntity)
	{
		Rpc(RPC_EL_SetQuantityTargetIntent, EL_Utils.GetRplId(sourceEntity), EL_Utils.GetRplId(destinationEntity));
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_SetQuantityTargetIntent(RplId sourceRplId, RplId destinationRplId)
	{
		IEntity sourceEntity = EL_Utils.FindEntityByRplId(sourceRplId);
		if (!sourceEntity) return;

		IEntity destinationEntity = EL_Utils.FindEntityByRplId(destinationRplId);
		if (!destinationEntity) return;

		// Validate that rpc sender is near the object to be manipulated
		if (!CanManipulate(sourceEntity, destinationEntity)) return;

		if (!m_mELQuantityTargetIntents) m_mELQuantityTargetIntents = new map<IEntity, IEntity>();
		m_mELQuantityTargetIntents.Set(sourceEntity, destinationEntity);
	}

	//------------------------------------------------------------------------------------------------
	protected bool CanManipulate(IEntity sourceEntity, IEntity destinationEntity)
	{
		const int maxManipulationRange = 10;
		vector rpcSenderPosition = GetOwner().GetOrigin();
		return (vector.Distance(sourceEntity.GetOrigin(), rpcSenderPosition) < maxManipulationRange &&
				vector.Distance(destinationEntity.GetOrigin(), rpcSenderPosition) < maxManipulationRange);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		// Restructure:
		// - Any specific target entity that has a quantity component also, try combine
		// - on fail see if the parent storage of a specific entity has a combineable item
		// - if no specific slot but a target storage entity (does not have a quantity component) try cobining it there
		// - if no specific slot and not target was declared just move it anywhere		
		
		EL_QuantityComponent quantitySource;
		if (Replication.IsServer()) quantitySource = EL_ComponentFinder<EL_QuantityComponent>.Find(item);
		if (quantitySource)
		{
			IEntity quantityDestionationEntity;
			if (m_mELQuantityTargetIntents) quantityDestionationEntity = m_mELQuantityTargetIntents.Get(item);
			if (quantityDestionationEntity)
			{
				m_mELQuantityTargetIntents.Remove(item);
				
				if (quantityDestionationEntity != item) // Seperate for fall through if destination is self so its transferred via super
				{
					// In case 0 quantity remains on the source it was deleted so no point in calling super.
					EL_QuantityComponent quantityDestination = EL_ComponentFinder<EL_QuantityComponent>.Find(quantityDestionationEntity);
					if (quantityDestination && quantityDestination.Combine(quantitySource) == 0) return;
				}
			}
			else
			{
				array<IEntity> combineableItems();
				EL_QuantityCombineablePredicate quantityCombinePredicate(quantitySource);
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
						// In case 0 quantity remains on the source it was deleted so no point in calling super or continue the distribution.
						if (quantityDestination.Combine(quantitySource) == 0) return;
					}
				}
			}
		}

		SCR_InventoryMenuUI inventoryMenu = SCR_InventoryMenuUI.Cast(ChimeraMenuBase.CurrentChimeraMenu());
		if (inventoryMenu) inventoryMenu.EL_Refresh(item);
		
		super.OnItemAdded(storageOwner, item);
	}
}

class EL_QuantityCombineablePredicate: InventorySearchPredicate
{
	protected EL_QuantityComponent m_pQuantitySource;
	//protected BaseInventoryStorageComponent m_pStorageRestriction;

	void EL_QuantityCombineablePredicate(notnull EL_QuantityComponent quantitySource/*, BaseInventoryStorageComponent storageRestriction = null*/)
	{
		m_pQuantitySource = quantitySource;
		//m_pStorageRestriction = storageRestriction;
		QueryComponentTypes.Insert(EL_QuantityComponent);
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return /*(!m_pStorageRestriction || m_pStorageRestriction == storage) && */(EL_QuantityComponent.Cast(queriedComponents[0])).CanCombine(m_pQuantitySource);
	}
}
