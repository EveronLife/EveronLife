modded class ScriptedInventoryStorageManagerComponent
{
	protected ref map<ResourceName, int> m_mELAmountCache;
	protected ref map<ResourceName, ref ScriptInvoker> m_mELItemCountInvokers;
	protected ref set<ResourceName> m_aELPendingItemRecountTypes;

	//------------------------------------------------------------------------------------------------
	//! Event invoker args (ResourceName prefab, int newAmount, int oldAmount)
	ScriptInvoker EL_GetOnAmountChanged(ResourceName prefab)
	{
		if (!prefab) return null;

		if (!m_mELAmountCache) m_mELAmountCache = new map<ResourceName, int>();
		if (!m_mELItemCountInvokers) m_mELItemCountInvokers = new map<ResourceName, ref ScriptInvoker>();

		ScriptInvoker invoker = m_mELItemCountInvokers.Get(prefab);
		if (!invoker)
		{
			invoker = new ScriptInvoker();
			m_mELItemCountInvokers.Set(prefab, invoker)
		}

		return invoker;
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_RecountItemsDebounced(ResourceName prefab)
	{
		if (!m_aELPendingItemRecountTypes) m_aELPendingItemRecountTypes = new set<ResourceName>();
		m_aELPendingItemRecountTypes.Insert(prefab);

		ScriptCallQueue callQueue = GetGame().GetCallqueue();
		if (callQueue.GetRemainingTime(EL_FlushRecountItems) == -1)
			callQueue.Call(EL_FlushRecountItems);
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_FlushRecountItems()
	{
		foreach (ResourceName prefab : m_aELPendingItemRecountTypes)
		{
			int newTotal = EL_InventoryUtils.GetAmount(this, prefab);
			if (newTotal == -1) continue;

			int previous = m_mELAmountCache.Get(prefab);
			m_mELAmountCache.Set(prefab, newTotal);

			ScriptInvoker invoker = m_mELItemCountInvokers.Get(prefab);
			if (invoker && newTotal != previous) invoker.Invoke(prefab, newTotal, previous);
		}

		m_aELPendingItemRecountTypes = null;
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemAdded(storageOwner, item);

		ResourceName prefab = EL_Utils.GetPrefabName(item);
		if (m_mELItemCountInvokers && m_mELItemCountInvokers.Contains(prefab))
		{
			EL_RecountItemsDebounced(prefab);

			EL_QuantityComponent quantity = EL_Component<EL_QuantityComponent>.Find(item);
			if (quantity) quantity.GetOnQuantityChanged().Insert(EL_HandleOnQuantityChanged);
		}
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnItemRemoved(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemRemoved(storageOwner, item);

		ResourceName prefab = EL_Utils.GetPrefabName(item);
		if (m_mELItemCountInvokers && m_mELItemCountInvokers.Contains(prefab))
		{
			EL_RecountItemsDebounced(prefab);

			EL_QuantityComponent quantity = EL_Component<EL_QuantityComponent>.Find(item);
			if (quantity) quantity.GetOnQuantityChanged().Remove(EL_HandleOnQuantityChanged);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected event void EL_HandleOnQuantityChanged(EL_QuantityComponent quantityComponent)
	{
		EL_RecountItemsDebounced(EL_Utils.GetPrefabName(quantityComponent.GetOwner()));
	}

	//------------------------------------------------------------------------------------------------
	void EL_RequestQuantitySplit(notnull IEntity sourceEntity, int splitSize = -1)
	{
		Rpc(RPC_EL_RequestQuantitySplit, EL_NetworkUtils.GetRplId(sourceEntity), splitSize);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_RequestQuantitySplit(RplId quantitySourceRplId, int splitSize)
	{
		EL_QuantityComponent quantitySource = EL_Component<EL_QuantityComponent>.Find(EL_NetworkUtils.FindEntityByRplId(quantitySourceRplId));
		if (!quantitySource || !EL_CanManipulate(quantitySource.GetOwner())) return;
		quantitySource.Split(splitSize);
	}

	//------------------------------------------------------------------------------------------------
	void EL_RequestQuantityTransfer(notnull IEntity sourceEntity, notnull IEntity destinationEntity, int amount = -1)
	{
		Rpc(RPC_EL_QuantityTransfer, EL_NetworkUtils.GetRplId(sourceEntity), EL_NetworkUtils.GetRplId(destinationEntity), amount);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_QuantityTransfer(RplId quantitySourceRplId, RplId quantityDestinationRplId, int amount)
	{
		EL_QuantityComponent quantitySource = EL_Component<EL_QuantityComponent>.Find(EL_NetworkUtils.FindEntityByRplId(quantitySourceRplId));
		if (!quantitySource || !EL_CanManipulate(quantitySource.GetOwner())) return;

		EL_QuantityComponent quantityDestination = EL_Component<EL_QuantityComponent>.Find(EL_NetworkUtils.FindEntityByRplId(quantityDestinationRplId));
		if (!quantityDestination || !EL_CanManipulate(quantityDestination.GetOwner())) return;

		quantityDestination.Combine(quantitySource, amount);
	}

	//------------------------------------------------------------------------------------------------
	void EL_SetTransferIntent(notnull IEntity sourceEntity, bool keepSeperate)
	{
		Rpc(RPC_EL_SetTransferIntent, EL_NetworkUtils.GetRplId(sourceEntity), keepSeperate);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_EL_SetTransferIntent(RplId sourceRplId, bool keepSeperate)
	{
		IEntity sourceEntity = EL_NetworkUtils.FindEntityByRplId(sourceRplId);
		if (!sourceEntity || !EL_CanManipulate(sourceEntity)) return;
		EL_QuantityComponent.SetTransferIntent(sourceEntity, keepSeperate);
	}

	//------------------------------------------------------------------------------------------------
	protected bool EL_CanManipulate(IEntity entity)
	{
		const int maxManipulationRange = 10;
		return (vector.Distance(entity.GetOrigin(), GetOwner().GetOrigin()) < maxManipulationRange);
	}
};
