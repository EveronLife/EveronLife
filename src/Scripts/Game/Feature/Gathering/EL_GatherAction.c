class EL_GatherAction : ScriptedUserAction
{
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

	protected string m_sDisplayName;

	//------------------------------------------------------------------------------------------------
	// User has performed the action
	// play a pickup sound and then add the correct amount to the users inventory
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!EL_NetworkUtils.IsOwner(pOwnerEntity)) return;
		
		SCR_InventoryStorageManagerComponent inventoryManager = EL_Component<SCR_InventoryStorageManagerComponent>.Find(pUserEntity);
		if (EL_InventoryUtils.AddAmount(inventoryManager, m_GatherItemPrefab, m_GatherAmount))
		{
			inventoryManager.RpcAsk_PlaySound(EL_NetworkUtils.GetRplId(pUserEntity), "SOUND_PICK_UP");
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
		if (!m_sDisplayName)
		{
			m_sDisplayName = "Unknown";
			UIInfo uiInfo = EL_UIInfoUtils.GetInfo(m_GatherItemPrefab);
			if (uiInfo) m_sDisplayName = uiInfo.GetName();
			m_sDisplayName = string.Format("Gather %1", m_sDisplayName);
		}

		outName = m_sDisplayName;
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

		// If not required we dont need to check anything
		if (!m_GatherToolRequirement) return true;

		SetCannotPerformReason("Requires item in hands");
		CharacterControllerComponent characterController = EL_Component<CharacterControllerComponent>.Find(user);
		if (characterController)
		{
			IEntity rightHandItem = characterController.GetRightHandItem();
			if (EL_Utils.GetPrefabName(rightHandItem) == m_GatherToolRequirement) return true;

			IEntity leftHandItem = characterController.GetAttachedGadgetAtLeftHandSlot();
			if (EL_Utils.GetPrefabName(leftHandItem) == m_GatherToolRequirement) return true;
		}

		if (m_CheckInventoryForToolRequirement)
		{
			SetCannotPerformReason("Requires item");
			if (EL_InventoryUtils.GetAmount(user, m_GatherToolRequirement) > 0) return true;
		}

		return false;
 	}
}
