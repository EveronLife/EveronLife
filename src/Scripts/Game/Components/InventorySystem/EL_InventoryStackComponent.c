[ComponentEditorProps()]
class EL_InventoryStackComponentClass: ScriptComponentClass
{
};

class EL_InventoryStackComponent : ScriptComponent
{
	[Attribute(defvalue: "5", uiwidget: UIWidgets.Auto, desc: "Inclusive maximal quantity the entity can be stacked")]
	private int m_QuantityMax;

	private int m_Quantity = 1;

	private InventoryItemComponent m_InventoryItemComponent;

	bool IsValid()
	{
		if (m_InventoryItemComponent == null)
		{
			return false;
		}

		return true;
	}

	int GetQuantity()
	{
		return m_Quantity;
	}

	int GetQuantityMax()
	{
		return m_QuantityMax;
	}

	float GetTotalWeight()
	{
		return m_InventoryItemComponent.GetTotalWeight();
	}

	EntityPrefabData GetPrefabData()
	{
		return GetOwner().GetPrefabData();
	}

	bool CanCombine(EL_InventoryStackComponent other)
	{
		if (GetPrefabData() != other.GetPrefabData())
		{
			return false;
		}
		
		if (GetTotalWeight() != other.GetTotalWeight())
		{
			return false;
		}
		
		int newQuantity = GetQuantity() + other.GetQuantity();

		if (newQuantity > GetQuantityMax())
		{
			return false;
		}

		return true;
	}

	bool Combine(EL_InventoryStackComponent other, SCR_InventoryStorageManagerComponent manager, SCR_InvCallBack cb = null, bool validate = true)
	{
		//! Check if everything about the two items are the same
		if (validate && !CanCombine(other))
		{
			return false;
		}
		
		int newQuantity = m_Quantity + other.m_Quantity;
				
		IEntity otherItem = other.GetOwner();
		if (false && manager) //! TODO: Investigate why 'TryDeleteItem' fails
		{
			if (otherItem && !manager.TryDeleteItem(otherItem))
			{
				Print("failed to delete");
				if (cb)
				{
					cb.InvokeOnFailed();
				}
				
				return false;
			}
		}
		else
		{
			delete otherItem;
		}
		
		m_Quantity = newQuantity;
		
		return true;
	}

	bool Split(EL_InventoryStackComponent other, SCR_InventoryStorageManagerComponent manager, SCR_InvCallBack cb = null)
	{
		return true;
	}

	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}

	protected override void EOnInit(IEntity owner) 
	{
		m_InventoryItemComponent = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
	}

	protected override bool RplLoad(ScriptBitReader reader)
	{
        reader.ReadInt(m_Quantity);

		return true;
	}

	protected override bool RplSave(ScriptBitWriter writer)
	{
       	writer.WriteInt(m_Quantity);

		return true;
	}
};
