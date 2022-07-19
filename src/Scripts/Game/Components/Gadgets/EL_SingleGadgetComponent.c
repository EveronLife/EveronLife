[EntityEditorProps(category: "GameScripted/Gadgets", description: "Gadget base", color: "0 0 255 255")]
class EL_SingleGadgetComponentClass: SCR_GadgetComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Gadget base class
//------------------------------------------------------------------------------------------------
class EL_SingleGadgetComponent : SCR_GadgetComponent
{			
	
	private bool m_bIsInit;
	private bool m_bIsLoading;
	
	//------------------------------------------------------------------------------------------------
	void SetLoading(bool isLoading)
	{
		m_bIsLoading = isLoading;
	}

	//------------------------------------------------------------------------------------------------
	//! Gadget mode change event
	//! \param mode is the target mode being switched to
	override void OnModeChanged(EGadgetMode mode, IEntity charOwner)
	{
		super.OnModeChanged(mode, charOwner);
		
		//If no longer in hand drop the item
		Print(mode);
		Print(m_bIsLoading);
		if (mode != EGadgetMode.IN_HAND && !m_bIsLoading)
		{
			//Item goes to storage first then to hand
			if (!m_bIsInit)
			{
				m_bIsInit = true;
				return;
			}
			m_bIsInit = false;
			
			SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(charOwner.FindComponent(SCR_InventoryStorageManagerComponent));
			if (inventoryManager.TryDeleteItem(GetOwner()))
				EL_Utils.SpawnEntityPrefab(GetOwner().GetPrefabData().GetPrefabName(), GetOwner().GetOrigin());
		}		
	}				
};