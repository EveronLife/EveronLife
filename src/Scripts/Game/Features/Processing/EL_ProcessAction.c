[BaseContainerProps()]
class EL_ProcessingInput
{
    [Attribute(ResourceName.Empty, UIWidgets.ResourcePickerThumbnail, "Prefab to Input", "et")]
	ResourceName m_InputPrefab;	
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Input/s amount per process")]
	int m_iInputAmount;
}

[BaseContainerProps()]
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
    ref array<ref EL_ProcessingOutput> m_aProcessingOutputs;
	
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
			foreach (EL_ProcessingInput m_aProcessingInput : m_aProcessingInputs)
			{
				if (item.GetPrefabData().GetPrefabName() == m_aProcessingInput.m_InputPrefab)
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
		
		array<string> CannotPerformArray = {};
		CannotPerformArray.Insert("Hello");
		
		bool CanPerform = true;
		
		foreach (EL_ProcessingInput m_aProcessingInput : m_aProcessingInputs)
		{
			if (CanPerform)
			{
				int inputPrefabsInInv = inventoryManager.GetDepositItemCountByResource(m_aProcessingInput.m_InputPrefab);
				
				if (!inventoryManager.CanInsertItem(m_OutputPrefabEntity))
				{
					SetCannotPerformReason("Inventory full");
					CanPerform = false;
				}
				SetCannotPerformReason("Can't find items");
				CanPerform = inputPrefabsInInv >= m_aProcessingInput.m_iInputAmount
			}
		}

		return (CanPerform);
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_OutputPrefabEntity = EL_Utils.SpawnEntityPrefab(m_OutputPrefab, vector.Zero);
	}
	
}

