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
		GetGame().GetCallqueue().Remove(EL_FlushRecountItems);
		GetGame().GetCallqueue().CallLater(EL_FlushRecountItems);
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

			PrintFormat(ToString()+"::OnAmountChanged(%1, %2, %3)", prefab, newTotal, previous);
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

			EL_QuantityComponent quantity = EL_ComponentFinder<EL_QuantityComponent>.Find(item);
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

			EL_QuantityComponent quantity = EL_ComponentFinder<EL_QuantityComponent>.Find(item);
			if (quantity) quantity.GetOnQuantityChanged().Remove(EL_HandleOnQuantityChanged);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected event void EL_HandleOnQuantityChanged(EL_QuantityComponent quantityComponent)
	{
		EL_RecountItemsDebounced(EL_Utils.GetPrefabName(quantityComponent.GetOwner()));
	}
}
