[EL_ComponentSaveDataType(EL_CharacterControllerComponentSaveDataClass, CharacterControllerComponent), BaseContainerProps()]
class EL_CharacterControllerComponentSaveDataClass : EL_ComponentSaveDataClass
{
}

[EL_DbName(EL_CharacterControllerComponentSaveData, "CharacterController")]
class EL_CharacterControllerComponentSaveData : EL_ComponentSaveData
{
	ECharacterStance m_eStance;
	string m_sLeftHandItemId;
	string m_sRightHandItemId;
	EEquipItemType m_eRightHandType;
	bool m_bRightHandRaised;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(worldEntityComponent);

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

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		//! >>> For playable character the same code is executed similary on the client in modded SCR_RespawnComponent. Transmitted via RPC <<<
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(worldEntityComponent);
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
		characterController.ForceStance(m_eStance);
		
		// Apply hand items
		IEntity rightHandEntity = persistenceManager.FindEntityByPersistentId(m_sRightHandItemId);
		if (rightHandEntity)
		{
			characterController.TryEquipRightHandItem(rightHandEntity, m_eRightHandType, false);
			characterController.SetWeaponRaised(m_bRightHandRaised);
		}

		// Left has to be second or else right hand weapon will remove left hand gadget again
		IEntity leftHandEntity = persistenceManager.FindEntityByPersistentId(m_sLeftHandItemId);
		if (leftHandEntity)
		{
			SCR_GadgetManagerComponent gadgetMgr = SCR_GadgetManagerComponent.GetGadgetManager(characterController.GetOwner());
			if (gadgetMgr) gadgetMgr.HandleInput(leftHandEntity, 1);
		}
		
		return true;
	}
}
