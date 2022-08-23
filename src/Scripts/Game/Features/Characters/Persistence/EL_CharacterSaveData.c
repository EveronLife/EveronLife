[EL_DbName(EL_CharacterSaveData, "Character"), BaseContainerProps()]
class EL_CharacterSaveData : EL_EntitySaveDataBase
{
	ECharacterStance m_eStance;
	
	string m_sLeftHandItemId;
	string m_sRightHandItemId;
	EEquipItemType m_eRightHandType;
	bool m_bRightHandRaised;
	
	override bool ReadFrom(notnull IEntity worldEntity)
	{
		if(!super.ReadFrom(worldEntity)) return false;
		
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(worldEntity.FindComponent(CharacterControllerComponent));
		if(characterController)
		{
			m_eStance = characterController.GetStance();
			
			m_sLeftHandItemId = EL_PersistenceComponent.GetPersistentId(characterController.GetAttachedGadgetAtLeftHandSlot());			
			m_sRightHandItemId = EL_PersistenceComponent.GetPersistentId(characterController.GetRightHandItem());
			if(m_sRightHandItemId)
			{
				m_eRightHandType = EEquipItemType.EEquipTypeGeneric;
			}
			else
			{
				BaseWeaponManagerComponent weaponManager = characterController.GetWeaponManagerComponent();
				if(weaponManager && weaponManager.GetCurrentSlot())
				{
					m_sRightHandItemId = EL_PersistenceComponent.GetPersistentId(weaponManager.GetCurrentSlot().GetWeaponEntity());
					m_eRightHandType = EEquipItemType.EEquipTypeWeapon;
					m_bRightHandRaised = characterController.IsWeaponRaised();
				}
			}
		}
		
		return true;
	}
	
	override bool ApplyTo(notnull IEntity worldEntity)
	{
		if(!super.ApplyTo(worldEntity)) return false;
		
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(worldEntity.FindComponent(CharacterControllerComponent));
		if(characterController)
		{
			characterController.ForceStance(m_eStance);
			
			EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
			
			if (m_sLeftHandItemId)
			{
				IEntity leftHandEntity = persistenceManager.FindEntityByPersistentId(m_sLeftHandItemId);
				if(leftHandEntity)
				{
					SCR_GadgetManagerComponent gadgetMgr = SCR_GadgetManagerComponent.GetGadgetManager(worldEntity);
					GetGame().GetScriptModule().Call(gadgetMgr, "InitComponent", false, null, worldEntity);
					gadgetMgr.HandleInput(leftHandEntity, 1);
				} 
			}
			
			if (m_sRightHandItemId)
			{
				IEntity rightHandEntity = persistenceManager.FindEntityByPersistentId(m_sRightHandItemId);
				if(rightHandEntity)
				{
					characterController.TryEquipRightHandItem(rightHandEntity, m_eRightHandType, false);
					characterController.SetWeaponRaised(m_bRightHandRaised);
				}
			}
		}
		
		return true;
	}	
	
	override protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if(!super.SerializationSave(saveContext)) return false;
		
		saveContext.WriteValue("m_eStance", m_eStance);
		saveContext.WriteValue("m_sLeftHandItemId", m_sLeftHandItemId);
		saveContext.WriteValue("m_sRightHandItemId", m_sRightHandItemId);
		saveContext.WriteValue("m_eRightHandType", m_eRightHandType);
		saveContext.WriteValue("m_bRightHandRaised", m_bRightHandRaised);
		
		return true;
	}
	
	override protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if(!super.SerializationLoad(loadContext)) return false;
		
		loadContext.ReadValue("m_eStance", m_eStance);
		loadContext.ReadValue("m_sLeftHandItemId", m_sLeftHandItemId);
		loadContext.ReadValue("m_sRightHandItemId", m_sRightHandItemId);
		loadContext.ReadValue("m_eRightHandType", m_eRightHandType);
		loadContext.ReadValue("m_bRightHandRaised", m_bRightHandRaised);
		
		return true;
	}
}
