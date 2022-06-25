[BaseContainerProps()]
class EL_ProcessingInput
{
    [Attribute(ResourceName.Empty, UIWidgets.ResourcePickerThumbnail, "Prefab to Input", "et")]
	ResourceName m_InputPrefab;	
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Input/s amount per process")]
	int m_iInputAmount;
	
	ref array<IEntity> foundItems = new array<IEntity>();
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
	private array<IEntity> GetAllInputItems(InventoryStorageManagerComponent inventory, )
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

		array<IEntity> allInputItems = GetAllInputItems(inventoryManager);

		foreach (EL_ProcessingInput m_aProcessingInput : m_aProcessingInputs) 
		{
			if (allInputItems.Count() < m_aProcessingInput.m_iInputAmount)
				return;
	
			for (int i = 0; i < m_aProcessingInput.m_iInputAmount; i++) 
			{
				inventoryManager.TryDeleteItem(allInputItems[i]);
			}
		}
		
		bool CanSpawnToStorage = true;
		
		foreach (EL_ProcessingOutput m_aProcessingOutput : m_aProcessingOutputs) 
		{
			for (int i = 0; i < m_aProcessingOutput.m_iOutputAmount; i++) 
			{
				CanSpawnToStorage = inventoryManager.TrySpawnPrefabToStorage(m_aProcessingOutput.m_OutputPrefab);
				if (!CanSpawnToStorage)
				{
					EntitySpawnParams spawnParams = new EntitySpawnParams();
					vector position[4];
					pUserEntity.GetWorldTransform(position);
					spawnParams.Transform = position;
					
					IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_aProcessingOutput.m_OutputPrefab), null, spawnParams);
				}
			}
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
				
				SetCannotPerformReason("Can't find items");
				CanPerform = inputPrefabsInInv >= m_aProcessingInput.m_iInputAmount
			}
		}

		return (CanPerform);
	}	
}

