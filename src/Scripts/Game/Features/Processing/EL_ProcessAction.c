class EL_ProcessingInput
{
    [Attribute(ResourceName.Empty, UIWidgets.ResourcePickerThumbnail, "Prefab to Input", "et")]
	ResourceName m_InputPrefab;	
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Input/s amount per process")]
	int m_iInputAmount;
}

class EL_ProcessingOutput
{
    [Attribute(ResourceName.Empty, UIWidgets.ResourcePickerThumbnail, "Prefab to Output", "et")]
	ResourceName m_OutputPrefab;	
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Output amount per process")]
	int m_iOutputAmount;
}

class EL_ProcessAction : ScriptedUserAction
{
	
	[Attribute("", UIWidgets.Object, "List of inputs")]
    ref array<ref EL_ProcessingInput> m_aProcessingInputs;
	
	[Attribute("", UIWidgets.Object, "List of outputs")]
    ref array<ref EL_ProcessingInput> m_aProcessingOutputs;
	
	private IEntity m_OutputPrefabEntity;
	
	
	//------------------------------------------------------------------------------------------------
	private array<IEntity> GetAllInputItems(InventoryStorageManagerComponent inventory)
	{
		array<IEntity> items = new array<IEntity>();
		array<IEntity> foundItems = new array<IEntity>();
		inventory.GetItems(items);
		
		//Add a parent each item prefab in the array
		foreach(ResourceName m_InputPrefab :  m_InputPrefabs)
		{
			//Check if item in recipe
			foreach (IEntity item : items)
			{
				if (item.GetPrefabData().GetPrefabName() == m_InputPrefab)
				{
					foundItems.Insert(item);
				}
			}
		}
		return foundItems;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> allFoundItems = GetAllInputItems(inventoryManager);
		
		if (allFoundItems.Count() < m_iInputAmount[0] || !inventoryManager.CanInsertItem(m_OutputPrefabEntity))
			return;
		
		for (int i = 0; i < m_iInputAmount[0]; i++) 
		{
			inventoryManager.TryDeleteItem(allFoundItems[i]);
		}
		
		for (int i = 0; i < m_iOutputAmount[0]; i++) 
		{
			inventoryManager.TrySpawnPrefabToStorage(m_OutputPrefabs[0]);
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		//Check player inventory
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		
		// Add a parent each item prefab in the array
		
		int inputPrefabsInInv = inventoryManager.GetDepositItemCountByResource(m_InputPrefabs[0]);
		
		if (!inventoryManager.CanInsertItem(m_OutputPrefabEntity))
		{
			SetCannotPerformReason("Inventory full");
			return false;
		}
		
		SetCannotPerformReason("Can't find items");
		
		return (inputPrefabsInInv >= m_iInputAmount[0]);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_OutputPrefabEntity = EL_Utils.SpawnEntityPrefab(m_OutputPrefabs[0], vector.Zero);
	}
	
}