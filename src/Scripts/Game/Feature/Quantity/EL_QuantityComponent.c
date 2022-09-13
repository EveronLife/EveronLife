class EL_QuantityComponentClass : ScriptComponentClass
{
	override static array<typename> CannotCombine(IEntityComponentSource src)
	{
		// Prevent user from adding multiple quantity components
		return {EL_QuantityComponent};
	}

	[Attribute(defvalue: "1", desc: "Inclusive maximal quantity this item can stack to. 0 means unlimited stack size.")]
	int m_iMaxQuantity;
}

class EL_QuantityComponent : ScriptComponent
{
	[RplProp(onRplName: "OnQuantityChanged")]
	protected int m_iQuantity = 1;

	protected RplComponent m_pReplication;

	int GetMaxQuantity()
	{
		int result = EL_QuantityComponentClass.Cast(GetComponentData(GetOwner())).m_iMaxQuantity;
		if (result == 0) result = int.MAX;
		return result;
	}

	int GetRemainingCapacity()
	{
		return GetMaxQuantity() - m_iQuantity;
	}

	int GetQuantity()
	{
		return m_iQuantity;
	}

	void AddQuantity(int quantity)
	{
		SetQuantity(m_iQuantity + quantity);
	}

	void SetQuantity(int quantity)
	{
		if (!m_pReplication.IsMaster()) return;

		m_iQuantity = quantity;
		Replication.BumpMe();
		OnQuantityChanged(); // Call on authority
	}

	void OnQuantityChanged()
	{
		SCR_InventoryMenuUI inventoryMenu = SCR_InventoryMenuUI.Cast(ChimeraMenuBase.CurrentChimeraMenu());
		if (inventoryMenu) inventoryMenu.EL_Refresh(GetOwner());
	}

	bool CanCombine(notnull EL_QuantityComponent other)
	{
		bool notSelf = this != other;
		bool prefabEqual = EL_Utils.GetPrefabName(GetOwner()) == EL_Utils.GetPrefabName(other.GetOwner());
		bool capacityLeft = m_iQuantity < GetMaxQuantity();
		return notSelf && prefabEqual && capacityLeft;
	}

	// Return the quantity left after combine
	int Combine(notnull EL_QuantityComponent other)
	{
		if (!CanCombine(other)) return other.GetQuantity();

		int transferQuantity = EL_Utils.MinInt(GetRemainingCapacity(), other.GetQuantity());

		AddQuantity(transferQuantity);

		if (other.GetQuantity() > transferQuantity)
		{
			// Reduce quantity of source
			other.AddQuantity(-transferQuantity);
		}
		else
		{
			// Source transferred entire quantity, so delete it
			SCR_EntityHelper.DeleteEntityAndChildren(other.GetOwner());
			return 0;
		}

		return other.GetQuantity();
	}

	override void OnPostInit(IEntity owner)
	{
		m_pReplication = RplComponent.Cast(owner.FindComponent(RplComponent));
	}
}
