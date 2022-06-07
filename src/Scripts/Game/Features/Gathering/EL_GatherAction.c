class EL_GatherAction : ScriptedUserAction
{
	[Attribute("", UIWidgets.EditBox, desc: "Display name of what is being gathered")]
	private string m_GatherItemDisplayName;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab what item is gathered")]
	private ResourceName m_GatherItemPrefab;
	
	[Attribute(defvalue:"1", UIWidgets.EditBox, desc: "Amount of items to receive")]
	private int m_AmountGathered;
	
	[Attribute("", UIWidgets.EditBox, desc: "Display name of what is needed")]
	private string m_RequiredItemDisplayName;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Item(s) required for gathering")]
	private ResourceName m_RequiredItemPrefab;
	
	[Attribute("", UIWidgets.CheckBox, desc: "Check entire inventory too")]
	private bool m_CheckInventory;
		
	
	private SCR_InventoryStorageManagerComponent m_InventoryManager;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		RplComponent replication = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		
		m_InventoryManager.PlayItemSound(replication.Id(), "SOUND_PICK_UP");
		
		//Spawn item
		for (int i = 0; i < m_AmountGathered; i++)
			m_InventoryManager.TrySpawnPrefabToStorage(m_GatherItemPrefab);
		
		//Show hint what to do with the gathered item
		EL_GameModeRoleplay.GetInstance().ShowInitalTraderHint();
	}
	
	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Gather %1", m_GatherItemDisplayName);
		return true;
	}
	
	override bool CanBePerformedScript(IEntity user)
 	{
		m_InventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		
		if (!m_RequiredItemPrefab)
			return true;
		
		if (m_CheckInventory)
		{
			SetCannotPerformReason(string.Format("Requires %1", m_RequiredItemDisplayName));
			
			array<BaseInventoryStorageComponent> outStorages = new array<BaseInventoryStorageComponent>();
			m_InventoryManager.GetStorages(outStorages);
			
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
			SetCannotPerformReason(string.Format("Requires %1 in hands", m_RequiredItemDisplayName));
			
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
