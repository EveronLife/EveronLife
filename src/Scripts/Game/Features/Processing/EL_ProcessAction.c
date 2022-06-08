class EL_ProcessAction : ScriptedUserAction
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.Auto, desc: "Recipe name to show on action")]
	private string m_RecipeName;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Input", "et")]
	ResourceName m_PrefabToInput;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Input amount per process")]
	private int m_InputAmount;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Output", "et")]
	ResourceName m_PrefabToOutput;		
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Output amount per process")]
	private int m_OutputAmount;
	
	private ref array<IEntity> m_FoundItems = {};
	private InventoryStorageManagerComponent m_InventoryManager;
	private string usingInventory;
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		for (int i = 0; i < m_InputAmount; i++) 
		{
			m_InventoryManager.TryDeleteItem(m_FoundItems.Get(i));
		}
		for (int i = 0; i < m_OutputAmount; i++) 
		{
			m_InventoryManager.TrySpawnPrefabToStorage(m_PrefabToOutput);
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Make %1 via %2", m_RecipeName, usingInventory);
		return true;
	}
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
 	{
		
		//Find near vehicle inventory items and check if they fit this reciepe.
		SetCannotPerformReason("Cant find items");
		
		//Check player inv.
		usingInventory = "Backpack";
		m_InventoryManager =  InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		GetAllInputItems(m_InventoryManager);
			
		if (m_FoundItems.Count() >= m_InputAmount)
			return true;	
		
		//Check vehicle inv.
		usingInventory = "Trunk";
		GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), 10, ContinueSearch, FilterVehicles, EQueryEntitiesFlags.ALL);
		
		return (m_FoundItems.Count() >= m_InputAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	//! TODO Only search owned vehicles.
	private bool ContinueSearch(IEntity ent)
	{
		m_InventoryManager = InventoryStorageManagerComponent.Cast(ent.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		GetAllInputItems(m_InventoryManager);
		
		return (m_FoundItems.Count() >= m_InputAmount);
	}
	
	//------------------------------------------------------------------------------------------------
	bool FilterVehicles(IEntity ent) 
	{
		return (ent.Type() == Vehicle);
	}
	
	//------------------------------------------------------------------------------------------------
	private void GetAllInputItems(notnull InventoryStorageManagerComponent inventory)
	{
		m_FoundItems.Clear();
		array<IEntity> items = {};
		inventory.GetItems(items);
		
		//Check if item in recipe
		foreach (IEntity item : items)
		{
			if (item.GetPrefabData().GetPrefabName() == m_PrefabToInput)
			{
				m_FoundItems.Insert(item);
			}
		}
	}
}