class EL_ProcessAction : ScriptedUserAction
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Input", "et")]
	ResourceName m_InputPrefab;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Input amount per process")]
	private int m_iInputAmount;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Output", "et")]
	ResourceName m_OutputPrefab;		
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Output amount per process")]
	private int m_iOutputAmount;
		
	private IEntity m_OutputPrefabEntity;
	
	
	//------------------------------------------------------------------------------------------------
	private array<IEntity> GetAllInputItems(InventoryStorageManagerComponent inventory)
	{
		array<IEntity> items = new array<IEntity>();
		array<IEntity> foundItems = new array<IEntity>();
		inventory.GetItems(items);
		
		//Check if item in recipe
		foreach (IEntity item : items)
		{
			if (item.GetPrefabData().GetPrefabName() == m_InputPrefab)
			{
				foundItems.Insert(item);
			}
		}
		
		return foundItems;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> allFoundItems = GetAllInputItems(inventoryManager);
		
		if (allFoundItems.Count() < m_iInputAmount || !inventoryManager.CanInsertItem(m_OutputPrefabEntity))
			return;
		
		for (int i = 0; i < m_iInputAmount; i++) 
		{
			inventoryManager.TryDeleteItem(allFoundItems[i]);
		}
		
		for (int i = 0; i < m_iOutputAmount; i++) 
		{
			inventoryManager.TrySpawnPrefabToStorage(m_OutputPrefab);
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		//Check player inventory
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		int inputPrefabsInInv = inventoryManager.GetDepositItemCountByResource(m_InputPrefab);
		
		if (!inventoryManager.CanInsertItem(m_OutputPrefabEntity))
		{
			SetCannotPerformReason("Inventory full");
			return false;
		}
		
		SetCannotPerformReason("Can't find items");
		
		return (inputPrefabsInInv >= m_iInputAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_OutputPrefabEntity = EL_Utils.SpawnEntityPrefab(m_OutputPrefab, vector.Zero);
	}
	
}