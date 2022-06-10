class EL_ProcessAction : ScriptedUserAction
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Input", "et")]
	ResourceName m_PrefabToInput;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Input amount per process")]
	private int m_InputAmount;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Output", "et")]
	ResourceName m_PrefabToOutput;		
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Output amount per process")]
	private int m_OutputAmount;
	
	private string usingInventory;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));

		array<IEntity> allFoundItems = GetAllInputItems(inventoryManager);
		
		for (int i = 0; i < m_InputAmount; i++) 
		{
			inventoryManager.TryDeleteItem(allFoundItems[i]);
		}
		
		for (int i = 0; i < m_OutputAmount; i++) 
		{
			inventoryManager.TrySpawnPrefabToStorage(m_PrefabToOutput);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		SetCannotPerformReason("Can't find items");
		
		//Check player inv.
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));

		return (inventoryManager && GetAllInputItems(inventoryManager).Count() >= m_InputAmount);	
	}
	
	//------------------------------------------------------------------------------------------------
	private array<IEntity> GetAllInputItems(InventoryStorageManagerComponent inventory)
	{
		array<IEntity> items = new array<IEntity>();
		array<IEntity> foundItems = new array<IEntity>();
		inventory.GetItems(items);
		
		//Check if item in recipe
		foreach (IEntity item : items)
		{
			if (item.GetPrefabData().GetPrefabName() == m_PrefabToInput)
			{
				foundItems.Insert(item);
			}
		}
		
		return foundItems;
	}	

}