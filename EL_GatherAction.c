class EL_GatherAction : ScriptedUserAction
{
	[Attribute("", UIWidgets.EditBox, desc: "Display name of what is being gathered")]
	private string m_GatherItemDisplayName;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab what item is gathered")]
	private ResourceName m_GatherItemPrefab;
	
	[Attribute("", UIWidgets.EditBox, desc: "Amount of items to receive")]
	private int m_AmountGathered;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Item(s) required for gathering")]
	private ResourceName m_RequiredItemPrefab;
	
	[Attribute("", UIWidgets.CheckBox, desc: "Check entire inventory too")]
	private bool m_CheckInventory;
		
	
	private SCR_InventoryStorageManagerComponent inventoryManager;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RplComponent replication = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		
		//SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		inventoryManager.PlayItemSound(replication.Id(), "SOUND_PICK_UP");
		
		//Spawn item
		for (int i = 0; i < m_AmountGathered; i++)
			inventoryManager.TrySpawnPrefabToStorage(m_GatherItemPrefab);
		
		//Show hint what to do with the gathered item - NOT sure if this works(?)
		//EL_GameModeRoleplay.GetInstance().ShowInitalTraderHint();
	}
	
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Gather %1", m_GatherItemDisplayName);
		return true;
	}
	
	override bool CanBePerformedScript(IEntity user)
 	{
		inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		
		if (!m_RequiredItemPrefab)
			return true;
		
		if (m_CheckInventory)
		{
			array<BaseInventoryStorageComponent> outStorages = new array<BaseInventoryStorageComponent>();
			inventoryManager.GetStorages(outStorages);
			
			foreach (BaseInventoryStorageComponent storage : outStorages)
			{
				array<IEntity> outItems = new array<IEntity>();
				storage.GetAll(outItems);
				
				foreach (IEntity item : outItems)
				{
					if (item.GetPrefabData().GetPrefabName() == m_RequiredItemPrefab)
						return true;
				}
			}
		}
		else // Check hands 
		{
			PlayerController playerController = PlayerController.Cast(GetGame().GetPlayerController());
			SCR_CharacterControllerComponent characterControllerComponent = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
			if (!characterControllerComponent)
				return false;
			
			IEntity rightHandItem = IEntity.Cast(characterControllerComponent.GetRightHandItem());
			if (rightHandItem && rightHandItem.GetPrefabData().GetPrefabName() == m_RequiredItemPrefab)
				return true;
			
			IEntity leftHandItem = IEntity.Cast(characterControllerComponent.GetAttachedGadgetAtLeftHandSlot());
			if (leftHandItem && leftHandItem.GetPrefabData().GetPrefabName() == m_RequiredItemPrefab)
				return true;
		}
		
		return false;
 	}
}