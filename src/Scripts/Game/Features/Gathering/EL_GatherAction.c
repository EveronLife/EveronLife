class EL_GatherAction : ScriptedUserAction
{
	[Attribute(desc: "Display name of what is being gathered", category: "General")]
	protected string m_GatherItemDisplayName;
	
	[Attribute(desc: "Prefab what item is gathered", category: "General")]
	protected ResourceName m_GatherItemPrefab;
	
	[Attribute(defvalue:"1", desc: "Amount of items to receive", category: "General")]
	protected int m_GatherAmount;
	
	[Attribute(desc: "Item required for gathering", category: "Requirements")]
	protected ResourceName m_GatherToolRequirement;
	
	[Attribute(desc: "Check entire inventory for required item too", category: "Requirements")]
	protected bool m_CheckInventoryForToolRequirement;
		
	[Attribute(desc: "Maximum amount of times to gather before resource has timeout", category: "Limits")]
	protected int m_GatherAmountMax;
	
	[Attribute(desc: "Amount of time needed until gathering attempts are restocked in ms", category: "Limits")]
	protected float m_GatherTimeout;
	
	protected int m_iRemainingGathers;
	protected float m_fNextQuantityRestock;
	
	//------------------------------------------------------------------------------------------------
	// User has performed the action
	// play a pickup sound and then add the correct amount to the users inventory
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		
		//Spawn items
		bool anyPickup;
		
		for (int i = 0; i < m_GatherAmount; i++)
		{
			if(inventoryManager.TrySpawnPrefabToStorage(m_GatherItemPrefab) && !anyPickup)
			{
				anyPickup = true;
			}	
		}
		
		if(anyPickup)
		{
			inventoryManager.PlayItemSound(pOwnerEntity, "SOUND_PICK_UP");
		}
		
		//Replenish gathering count
		if(m_iRemainingGathers <= 0) m_iRemainingGathers = m_GatherAmountMax;
		
		//Consume one gathering
		m_iRemainingGathers--;
		
		//Initalize timeout if resource depleted
		if(m_iRemainingGathers <= 0)
		{
			m_fNextQuantityRestock = Replication.Time() + m_GatherTimeout;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Formats name for action when hovering
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Gather %1", m_GatherItemDisplayName);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	// Checks if a required item has been set in the Editor
	// If so, check if its in the users inventory/hands depending on settings set
	override bool CanBePerformedScript(IEntity user)
 	{
		if(m_fNextQuantityRestock > Replication.Time())
		{
			int secondsLeft = (m_fNextQuantityRestock - Replication.Time()) / 1000;
			SetCannotPerformReason(string.Format("Please wait %1 seconds", secondsLeft + 1)); //+1 to avoid 0 seconds left.
			return false;
		}
		
		if (!m_GatherToolRequirement) // If not required we dont need to check anything
			return true;
		
		if (m_CheckInventoryForToolRequirement) // Just check the inventory
		{
			SetCannotPerformReason("Requires item");
			
			SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
			
			array<IEntity> outItems();
			inventoryManager.GetItems(outItems);
			
			foreach (IEntity item : outItems)
			{
				if (item.GetPrefabData().GetPrefabName() == m_GatherToolRequirement)
					return true;
			}
		}
		else // Check hands 
		{
			SetCannotPerformReason("Requires item in hands");
			
			SCR_CharacterControllerComponent characterControllerComponent = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
			if (!characterControllerComponent)
				return false;
			
			IEntity rightHandItem = IEntity.Cast(characterControllerComponent.GetRightHandItem());
			if (rightHandItem && rightHandItem.GetPrefabData().GetPrefabName() == m_GatherToolRequirement)
				return true;
			
			IEntity leftHandItem = IEntity.Cast(characterControllerComponent.GetAttachedGadgetAtLeftHandSlot());
			if (leftHandItem && leftHandItem.GetPrefabData().GetPrefabName() == m_GatherToolRequirement)
				return true;
		}
		
		return false;
 	}
}
