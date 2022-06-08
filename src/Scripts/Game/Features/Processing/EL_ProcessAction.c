class EL_ProcessAction : ScriptedUserAction
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.Auto, desc: "Output amount per process", category: "Spawn Area")]
	private string m_ReciepeName;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Spawn", "et", category: "Spawn Area")]
	ResourceName m_PrefabToInput;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Input amount per process", category: "Spawn Area")]
	private int m_InputAmount;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Spawn", "et", category: "Spawn Area")]
	ResourceName m_PrefabToOutput;		
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Output amount per process", category: "Spawn Area")]
	private int m_OutputAmount;
	
	private ref array<IEntity> m_FoundItems = {};
	InventoryStorageManagerComponent m_InventoryManager;
	private string usingInventory = "Player";
	
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
		outName = string.Format("Make %1 via %2", m_ReciepeName, usingInventory);
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
		
		//Check if item in reciepe
		foreach (IEntity item : items)
		{
			if (item.GetPrefabData().GetPrefabName() == m_PrefabToInput)
			{
				m_FoundItems.Insert(item);
			}
		}
	}
}