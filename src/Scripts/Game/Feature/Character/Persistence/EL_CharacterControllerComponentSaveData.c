[EL_ComponentSaveDataType(SCR_CharacterControllerComponent), BaseContainerProps()]
class EL_CharacterControllerComponentSaveDataClass : EL_ComponentSaveDataClass
{
	override array<typename> Requires()
	{
		return {EL_BaseInventoryStorageComponentSaveDataClass};
	}
};

[EL_DbName.Automatic()]
class EL_CharacterControllerComponentSaveData : EL_ComponentSaveData
{
	ECharacterStance m_eStance;
	string m_sLeftHandItemId;
	string m_sRightHandItemId;
	EEquipItemType m_eRightHandType;
	bool m_bRightHandRaised;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(component);

		m_eStance = characterController.GetStance();
		m_sLeftHandItemId = EL_PersistenceComponent.GetPersistentId(characterController.GetAttachedGadgetAtLeftHandSlot());
		m_sRightHandItemId = EL_PersistenceComponent.GetPersistentId(characterController.GetRightHandItem());
		if (m_sRightHandItemId)
		{
			m_eRightHandType = EEquipItemType.EEquipTypeGeneric;
		}
		else
		{
			BaseWeaponManagerComponent weaponManager = characterController.GetWeaponManagerComponent();
			if (weaponManager && weaponManager.GetCurrentSlot())
			{
				m_sRightHandItemId = EL_PersistenceComponent.GetPersistentId(weaponManager.GetCurrentSlot().GetWeaponEntity());
				m_eRightHandType = EEquipItemType.EEquipTypeWeapon;
				m_bRightHandRaised = characterController.IsWeaponRaised();
			}
		}

		if (attributes.m_bTrimDefaults &&
			m_eStance == ECharacterStance.STAND &&
			!m_sLeftHandItemId &&
			!m_sRightHandItemId &&
			m_eRightHandType == EEquipItemType.EEquipTypeNone &&
			!m_bRightHandRaised) return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		EL_EApplyResult result = EL_EApplyResult.OK;

		// Apply stance
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(component);
		if (characterController.GetStance() != m_eStance)
		{
			switch (m_eStance)
			{
				case ECharacterStance.STAND:
				{
					characterController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOERECTED);
					break;
				}
				case ECharacterStance.CROUCH:
				{
					characterController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
					break;
				}
				case ECharacterStance.PRONE:
				{
					characterController.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOPRONE);
					break;
				}
			}
		}

		// Apply hand items
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		IEntity rightHandEntity = persistenceManager.FindEntityByPersistentId(m_sRightHandItemId);
		if (rightHandEntity)
		{
			BaseWeaponManagerComponent weaponManager = EL_Component<BaseWeaponManagerComponent>.Find(characterController.GetOwner());
			if (weaponManager)
			{
				EL_DeferredApplyResult.AddPending(this, "CharacterControllerComponentSaveData::RightHandItemEquipped");
				weaponManager.m_OnWeaponChangeCompleteInvoker.Insert(OnWeaponChangeComplete);
				result = EL_EApplyResult.AWAIT_COMPLETION;
			}

			characterController.TryEquipRightHandItem(rightHandEntity, m_eRightHandType, false);
		}
		else if (m_sLeftHandItemId)
		{
			// No weapon so gadget can be equipped as soon as manager is ready
			SCR_GadgetManagerComponent gadgetMananger = SCR_GadgetManagerComponent.GetGadgetManager(characterController.GetOwner());
			if (gadgetMananger)
			{
				EL_DeferredApplyResult.AddPending(this, "CharacterControllerComponentSaveData::GadgetEquipped");
				gadgetMananger.GetOnGadgetInitDoneInvoker().Insert(OnGadgetInitDone);
				result = EL_EApplyResult.AWAIT_COMPLETION;
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected void OnWeaponChangeComplete(BaseWeaponComponent newWeaponSlot)
	{
		IEntity owner = newWeaponSlot.GetOwner();
		SCR_CharacterControllerComponent characterController = EL_Component<SCR_CharacterControllerComponent>.Find(owner);
		
		BaseWeaponManagerComponent weaponManager = EL_Component<BaseWeaponManagerComponent>.Find(owner);
		weaponManager.m_OnWeaponChangeCompleteInvoker.Remove(OnWeaponChangeComplete);

		if (m_bRightHandRaised)
		{
			EL_DeferredApplyResult.AddPending(this, "CharacterControllerComponentSaveData::WeaponRaised");
			characterController.SetWeaponRaised(m_bRightHandRaised);
			// Hardcode weapon raise complete after fixed amount of time until we have a nice even for OnRaiseFinished or something
			GetGame().GetCallqueue().CallLater(OnRaiseFinished, 1000);
		}

		if (m_sLeftHandItemId)
		{
			// By the time a weapon was equipped the manager will be ready so we can take gadget to hands now
			EL_DeferredApplyResult.AddPending(this, "CharacterControllerComponentSaveData::GadgetEquipped");
			OnGadgetInitDone(owner, SCR_GadgetManagerComponent.GetGadgetManager(owner));
		}

		EL_DeferredApplyResult.SetFinished(this, "CharacterControllerComponentSaveData::RightHandItemEquipped");
	}

	//------------------------------------------------------------------------------------------------
	protected void OnRaiseFinished()
	{
		EL_DeferredApplyResult.SetFinished(this, "CharacterControllerComponentSaveData::WeaponRaised");
	}

	//------------------------------------------------------------------------------------------------
	protected void OnGadgetInitDone(IEntity owner, SCR_GadgetManagerComponent gadgetMananger)
	{
		IEntity leftHandEntity = EL_PersistenceManager.GetInstance().FindEntityByPersistentId(m_sLeftHandItemId);
		if (!leftHandEntity)
			return;

		gadgetMananger.HandleInput(leftHandEntity, 1);

		SCR_CharacterControllerComponent characterController = EL_Component<SCR_CharacterControllerComponent>.Find(owner);
		characterController.m_OnGadgetStateChangedInvoker.Insert(OnGadgetStateChanged);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnGadgetStateChanged(IEntity gadget, bool isInHand, bool isOnGround)
	{
		if (isInHand)
			EL_DeferredApplyResult.SetFinished(this, "CharacterControllerComponentSaveData::GadgetEquipped");
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_CharacterControllerComponentSaveData otherData = EL_CharacterControllerComponentSaveData.Cast(other);
		return m_eStance == otherData.m_eStance &&
			m_sLeftHandItemId == otherData.m_sLeftHandItemId &&
			m_sRightHandItemId == otherData.m_sRightHandItemId &&
			m_eRightHandType == otherData.m_eRightHandType &&
			m_bRightHandRaised == otherData.m_bRightHandRaised;
	}
};
