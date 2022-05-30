[ComponentEditorProps()]
class EL_InventoryStackComponentClass: ScriptComponentClass
{
};

class EL_InventoryStackComponent : ScriptComponent
{
	[Attribute(defvalue: "5", uiwidget: UIWidgets.Auto, desc: "Inclusive maximal quantity the entity can be stacked")]
	private int m_QuantityMax;

	[RplProp()]
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

	void SetQuantity(int quantity)
	{
		m_Quantity = quantity;
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
		if (!DeleteItem(otherItem, manager, cb))
		{
			return false;
		}
		
		m_Quantity = newQuantity;
		
		return true;
	}

	bool Split(BaseInventoryStorageComponent destination, SCR_InventoryStorageManagerComponent manager, SCR_InvCallBack cb = null)
	{
		IEntity entity = GetOwner();
		IEntity newEntity = null;

		int quantity = GetQuantity();
		int quantityA = Math.Floor(0.5 * quantity);
		int quantityB = Math.Ceil(0.5 * quantity);

		//! If the destination is the same and the quantity doesn't change then early terminate
		if (destination == GetOwningStorage() && (quantityA == 0 || quantityB == 0))
		{
			return false;
		}

		EntityPrefabData entityData = entity.GetPrefabData();
		if (!entityData)
		{
			return false;
		}

		Resource resource = Resource.Load(entityData.GetPrefabName());
		if (!resource.IsValid())
		{
			return false;
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
			return false;
		}
		
		EL_InventoryStackComponent newStackComponent = EL_InventoryStackComponent.Cast(newEntity.FindComponent(EL_InventoryStackComponent));
		if (!newStackComponent)
		{
			return false;
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

			if (!EL_InventoryStackComponent.DeleteItem(entity, null, null))
			{
				return false;
			}

			newStackComponent.SetQuantity(quantity);
		}
		else
		{
			SetQuantity(quantityA);
			newStackComponent.SetQuantity(quantityB);
		}

		return true;
	}

	static bool DeleteItem(IEntity item, SCR_InventoryStorageManagerComponent manager, SCR_InvCallBack cb = null)
	{
		if (!item)
		{
			return false;
		}

		if (false && manager) //! TODO: Investigate why 'TryDeleteItem' fails
		{
			if (!manager.TryDeleteItem(item))
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
			delete item;
		}

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
