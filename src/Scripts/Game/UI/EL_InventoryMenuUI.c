modded class SCR_InventoryMenuUI
{
	override void OnAction(SCR_NavigationButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{
		switch (action)
		{
		case "EL_Inventory_SplitStack":
		{
			EL_SplitStack();
			return;
		}
		}

		super.OnAction(comp, action, pParentStorage, traverseStorageIndex);
	}

	void EL_SplitStack()
	{
		if (!m_pFocusedSlotUI)
		{
			return;
		}

		EL_InventoryStackComponent stackComponent = m_pFocusedSlotUI.EL_GetInventoryStackComponent();
		if (!stackComponent)
		{
			return;
		}

		IEntity entity = stackComponent.GetOwner();
		if (!entity)
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

		EntitySpawnParams params();
		m_Player.GetWorldTransform(params.Transform);

		IEntity newEntity = GetGame().SpawnEntityPrefab(resource, entity.GetWorld(), params);
		if (!newEntity)
		{
			return;
		}
		
		EL_InventoryStackComponent newStackComponent = EL_InventoryStackComponent.Cast(newEntity.FindComponent(EL_InventoryStackComponent));
		if (!newStackComponent)
		{
			return;
		}

		int quantity = stackComponent.GetQuantity();
		int quantityA = Math.Floor(0.5 * quantity);
		int quantityB = Math.Ceil(0.5 * quantity);
		
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
				return;
			}

			newStackComponent.SetQuantity(quantity);
		}
		else
		{
			stackComponent.SetQuantity(quantityA);
			newStackComponent.SetQuantity(quantityB);
		}

		ShowStoragesList();
		ShowAllStoragesInList();
		RefreshLootUIListener();
		RefreshUISlotStorages();
		NavigationBarUpdate();
	}

	override void NavigationBarUpdate()
	{
		super.NavigationBarUpdate();
		
		m_pNavigationBar.SetButtonEnabled("ButtonDropStack", true);
	}
};
