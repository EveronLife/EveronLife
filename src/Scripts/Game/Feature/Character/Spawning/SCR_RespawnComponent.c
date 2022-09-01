class EL_RespawnCharacterState
{
	ECharacterStance m_eStance;

	RplId m_pLeftHandItemRplId;
	RplId m_pRightHandItemRplId;
	EEquipItemType m_eRightHandType;
	bool m_bRightHandRaised;

	ref array<RplId> m_aQuickBarRplIds;

	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx hint, ScriptBitSerializer packet)
	{
		snapshot.Serialize(packet, 20);

		int count;
		snapshot.SerializeInt(count);
		packet.SerializeInt(count);
		for (int nId = 0; nId < count; nId++)
		{
			RplId id;
			snapshot.SerializeInt(id);
			packet.SerializeRplId(id);
		}
	}

	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx hint, SSnapSerializerBase snapshot)
	{
		if (!snapshot.Serialize(packet, 20)) return false;

		int count;
		packet.SerializeInt(count);
		snapshot.SerializeInt(count);
		for (int nId = 0; nId < count; nId++)
		{
			RplId id;
			packet.SerializeRplId(id);
			snapshot.SerializeInt(id);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx hint)
	{
		return lhs.CompareSnapshots(rhs, 24); // 5 props + quick bar array count = 6 * 4 byte
	}

	//------------------------------------------------------------------------------------------------
	static bool PropCompare(EL_RespawnCharacterState prop, SSnapSerializerBase snapshot, ScriptCtx hint)
	{
		return snapshot.CompareInt(prop.m_eStance)
			&& snapshot.CompareInt(prop.m_pLeftHandItemRplId)
			&& snapshot.CompareInt(prop.m_pRightHandItemRplId)
			&& snapshot.CompareInt(prop.m_eRightHandType)
			&& snapshot.CompareBool(prop.m_bRightHandRaised)
			&& snapshot.Compare(prop.m_aQuickBarRplIds, prop.m_aQuickBarRplIds.Count() + 1);
	}

	//------------------------------------------------------------------------------------------------
	static bool Extract(EL_RespawnCharacterState prop, ScriptCtx hint, SSnapSerializerBase snapshot)
	{
		snapshot.SerializeInt(prop.m_eStance);
		snapshot.SerializeInt(prop.m_pLeftHandItemRplId);
		snapshot.SerializeInt(prop.m_pRightHandItemRplId);
		snapshot.SerializeInt(prop.m_eRightHandType);
		snapshot.SerializeBool(prop.m_bRightHandRaised);

		int count = prop.m_aQuickBarRplIds.Count();
		snapshot.SerializeInt(count);
		foreach (RplId rplId : prop.m_aQuickBarRplIds)
		{
			snapshot.SerializeInt(rplId);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx hint, EL_RespawnCharacterState prop)
	{
		snapshot.SerializeInt(prop.m_eStance);
		snapshot.SerializeInt(prop.m_pLeftHandItemRplId);
		snapshot.SerializeInt(prop.m_pRightHandItemRplId);
		snapshot.SerializeInt(prop.m_eRightHandType);
		snapshot.SerializeBool(prop.m_bRightHandRaised);

		int count;
		snapshot.SerializeInt(count);
		prop.m_aQuickBarRplIds = new array<RplId>();
		prop.m_aQuickBarRplIds.Resize(count);
		for (int nId = 0; nId < count; nId++)
		{
			RplId id;
			snapshot.SerializeInt(id);
			prop.m_aQuickBarRplIds.Set(nId, id);
		}

		return true;
	}
}

modded class SCR_RespawnComponent
{
	protected ref EL_RespawnCharacterState EL_m_pRespawnCharacterState;

	//------------------------------------------------------------------------------------------------
	void SetRespawnCharacterState(EL_RespawnCharacterState state)
	{
		Rpc(Rpc_SetRespawnCharacterState, state);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void Rpc_SetRespawnCharacterState(EL_RespawnCharacterState state)
	{
		EL_m_pRespawnCharacterState = state;
	}

	//------------------------------------------------------------------------------------------------
	override protected void RpcAsk_NotifyOnPlayerSpawned()
	{
		super.RpcAsk_NotifyOnPlayerSpawned();
		GetGame().GetCallqueue().Call(ApplyDeferredPhase1);
	}

	//------------------------------------------------------------------------------------------------
	//! Start applying save-data clientside after taking control of the character
	protected void ApplyDeferredPhase1()
	{
		if (!EL_m_pRespawnCharacterState) return;

		ChimeraCharacter character = ChimeraCharacter.Cast(m_PlayerController.GetControlledEntity());
		if (!character) return;

		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(character.FindComponent(CharacterControllerComponent));
		if (!characterController) return;

		// Apply stance
		characterController.ForceStance(EL_m_pRespawnCharacterState.m_eStance);

		// Apply quick bar
		SCR_CharacterInventoryStorageComponent inventoryStorage = SCR_CharacterInventoryStorageComponent.Cast(character.FindComponent(SCR_CharacterInventoryStorageComponent));
		inventoryStorage.EL_Rpc_UpdateQuickSlotItems(EL_m_pRespawnCharacterState.m_aQuickBarRplIds);

		// Split off into second phase to avoid crash when handitem + stance happen at the same time
		GetGame().GetCallqueue().CallLater(ApplyDeferredPhase2, 100, false, character, characterController);
	}

	//------------------------------------------------------------------------------------------------
	protected void ApplyDeferredPhase2(ChimeraCharacter character, CharacterControllerComponent characterController)
	{
		// Apply hand items
		IEntity rightHandEntity = EL_Utils.FindEntityByRplId(EL_m_pRespawnCharacterState.m_pRightHandItemRplId);
		if (rightHandEntity)
		{
			characterController.TryEquipRightHandItem(rightHandEntity, EL_m_pRespawnCharacterState.m_eRightHandType, false);
			characterController.SetWeaponRaised(EL_m_pRespawnCharacterState.m_bRightHandRaised);
		}

		// Left has to be second or else right hand weapon will remove left hand gadget again
		IEntity leftHandEntity = EL_Utils.FindEntityByRplId(EL_m_pRespawnCharacterState.m_pLeftHandItemRplId);
		if (leftHandEntity)
		{
			SCR_GadgetManagerComponent gadgetMgr = SCR_GadgetManagerComponent.GetGadgetManager(character);
			if (gadgetMgr) gadgetMgr.HandleInput(leftHandEntity, 1);
		}
	}
}
