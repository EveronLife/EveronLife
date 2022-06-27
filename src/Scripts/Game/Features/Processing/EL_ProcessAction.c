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


	//------------------------------------------------------------------------------------------------
	private array<IEntity> GetAllInputItems(InventoryStorageManagerComponent inventory)
	{
		array<IEntity> items = new array<IEntity>();
		array<IEntity> foundItems = new array<IEntity>();
		inventory.GetItems(items);

		//Check if item in recipe
		foreach (IEntity item : items)
		{
			foreach (EL_ProcessingInput processingInput : m_aProcessingInputs)
			{
				if (item.GetPrefabData().GetPrefabName() == processingInput.m_InputPrefab)
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

		array<IEntity> allInputItems = GetAllInputItems(inventoryManager);

		foreach (EL_ProcessingInput processingInput : m_aProcessingInputs) 
		{
			if (allInputItems.Count() < processingInput.m_iInputAmount)
				return;
	
			for (int i = 0; i <= processingInput.m_iInputAmount; i++) 
			{
				inventoryManager.TryDeleteItem(allInputItems[i]);
			}
		}
		
		bool bCanSpawnToStorage = true;
		
		foreach (EL_ProcessingOutput processingOutput : m_aProcessingOutputs) 
		{
			for (int i = 0; i < processingOutput.m_iOutputAmount; i++) 
			{
				bCanSpawnToStorage = inventoryManager.TrySpawnPrefabToStorage(processingOutput.m_OutputPrefab);
				if (!bCanSpawnToStorage)
				{			
					EL_Utils.SpawnEntityPrefab(processingOutput.m_OutputPrefab, pUserEntity.GetOrigin());
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		//Check player inventory
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
	
		
		bool bCanPerform = true;
		
		foreach (EL_ProcessingInput processingInput : m_aProcessingInputs)
		{
			if (bCanPerform)
			{
				int inputPrefabsInInv = inventoryManager.GetDepositItemCountByResource(processingInput.m_InputPrefab);
				
				SetCannotPerformReason("Can't find items");
				bCanPerform = inputPrefabsInInv >= processingInput.m_iInputAmount
			}
		}

		return (bCanPerform);
	}	
}

