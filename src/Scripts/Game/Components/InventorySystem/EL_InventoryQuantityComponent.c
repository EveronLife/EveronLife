[ComponentEditorProps()]
class EL_InventoryQuantityComponentClass: ScriptComponentClass
{
};

class EL_InventoryQuantityComponent : ScriptComponent
{
	[Attribute(defvalue: "5", uiwidget: UIWidgets.Auto, desc: "Inclusive maximal quantity the entity can be stacked. 0 equals no maximum.")]
	private int m_QuantityMax;

	[RplProp(onRplName: "CallOnQuantityChanged")]
	private int m_Quantity = 1;
	private int m_QuantityPrev = 1;

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

	//! Doesn't destory the entity when quantity equals 0
	void SetQuantity(int quantity)
	{
		m_Quantity = Math.Max(quantity, 0.0);

		if (m_QuantityMax >= 1)
		{
			m_Quantity = Math.Min(m_Quantity, m_QuantityMax);
		}

		CallOnQuantityChanged();
		Replication.BumpMe();
	}

	//! Returns consumed quantity value
	int AddQuantity(int quantity)
	{
		int start = m_Quantity;

		SetQuantity(m_Quantity + quantity);

		return m_Quantity - start;
	}

	private void CallOnQuantityChanged()
	{
		int prev = m_QuantityPrev;
		m_QuantityPrev = m_Quantity;
		OnQuantityChanged(prev);
	}
	
	//! Doesn't get called when entity is destroyed as a result of the quantity equalling 0 
	void OnQuantityChanged(int previous)
	{
		//Print("OnQuantityChanged: " + this);
		//Print("Prev: " + previous);
		//Print("Curr: " + GetQuantity());

		SCR_InventoryMenuUI menu = SCR_InventoryMenuUI.Cast(ChimeraMenuBase.CurrentChimeraMenu());
		if (menu)
		{
			menu.EL_Refresh();
		}
	}

	int GetQuantityMax()
	{
		return m_QuantityMax;
	}

	float GetTotalWeight()
	{
		return m_InventoryItemComponent.GetTotalWeight();
	}

	BaseInventoryStorageComponent GetOwningStorage()
	{
		InventoryStorageSlot parentSlot = m_InventoryItemComponent.GetParentSlot();
		if (parentSlot)
		{
			return parentSlot.GetStorage();
		}

		return null;
	}

	EntityPrefabData GetPrefabData()
	{
		return GetOwner().GetPrefabData();
	}

	bool CanCombine(EL_InventoryQuantityComponent other)
	{
		if (this == other)
		{
			return false;
		}

		if (GetPrefabData() != other.GetPrefabData())
		{
			return false;
		}
		
		if (GetTotalWeight() != other.GetTotalWeight())
		{
			return false;
		}

		return true;
	}

	void Combine(EL_InventoryQuantityComponent other, SCR_InventoryStorageManagerComponent manager)
	{
		manager.EL_Combine(this, other);
	}

	void Split(BaseInventoryStorageComponent destination, SCR_InventoryStorageManagerComponent manager)
	{
		manager.EL_Split(this, destination);
	}

	void LocalCombine(EL_InventoryQuantityComponent other)
	{
		//! Check if everything about the two items are the same
		if (!CanCombine(other))
		{
			return;
		}

		int requested = other.GetQuantity();
		int consumed = AddQuantity(requested);
		if (requested == consumed)
		{	
			IEntity otherItem = other.GetOwner();
			RplComponent.DeleteRplEntity(otherItem, false);
		}
		else
		{
			other.SetQuantity(requested - consumed);
		}
	}
	
	void LocalSplit(BaseInventoryStorageComponent destination, SCR_InventoryStorageManagerComponent manager, int quantityA, int quantityB)
	{
		IEntity entity = GetOwner();
		IEntity newEntity = null;

		//! If the destination is the same and the quantity doesn't change then early terminate
		if (destination == GetOwningStorage() && (quantityA == 0 || quantityB == 0))
		{
			return;
		}

		EntityPrefabData entityData = entity.GetPrefabData();
		if (!entityData)
		{
			return;
		}

		Resource resource = Resource.Load(entityData.GetPrefabName());
		if (!resource.IsValid())
		{
			return;
		}

		IEntity player = manager.GetOwner();

		EntitySpawnParams params();
		player.GetWorldTransform(params.Transform);

		newEntity = GetGame().SpawnEntityPrefab(resource, entity.GetWorld(), params);

		if (destination && newEntity)
		{
			manager.TryInsertItemInStorage(newEntity, destination);
		}

		if (!newEntity)
		{
			return;
		}
		
		EL_InventoryQuantityComponent newQuantityComponent = EL_InventoryQuantityComponent.Cast(newEntity.FindComponent(EL_InventoryQuantityComponent));
		if (!newQuantityComponent)
		{
			return;
		}
		
		//! If either is zero then we are removing the item from the inventory
		if (quantityB == 0 || quantityA == 0)
		{
			if (quantityA == 0)
			{
				quantityA = quantityB;
			}

			if (quantityA == 0)
			{
				quantityA = 1;
			}

			newQuantityComponent.SetQuantity(quantityA);

			RplComponent.DeleteRplEntity(entity, false);
		}
		else
		{
			SetQuantity(quantityA);
			newQuantityComponent.SetQuantity(quantityB);
		}
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
