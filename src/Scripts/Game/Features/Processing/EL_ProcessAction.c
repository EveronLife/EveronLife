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

	ref SCR_PrefabNamePredicate m_pPrefabNamePredicate = new SCR_PrefabNamePredicate();

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		InventoryStorageManagerComponent inventoryManager = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));

		foreach (EL_ProcessingInput processingInput : m_aProcessingInputs)
		{
			//Set search to new input prefab
			m_pPrefabNamePredicate.prefabName = processingInput.m_InputPrefab;

			for (int i = 0; i < processingInput.m_iInputAmount; i++)
			{
				inventoryManager.TryDeleteItem(inventoryManager.FindItem(m_pPrefabNamePredicate));
			}
		}

		bool bCanSpawnToStorage;

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

		foreach (EL_ProcessingInput processingInput : m_aProcessingInputs)
		{
			int inputPrefabsInInv = inventoryManager.GetDepositItemCountByResource(processingInput.m_InputPrefab);
			if (inputPrefabsInInv < processingInput.m_iInputAmount)
				return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		SetCannotPerformReason("Can't find items");
	}
}
