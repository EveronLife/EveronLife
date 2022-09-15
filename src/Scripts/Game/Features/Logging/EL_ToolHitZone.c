class EL_ToolHitZone : ScriptedHitZone
{

	[Attribute("", UIWidgets.ResourceNamePicker, "Particle to spawn on hit", "et")]
	ResourceName m_HitVFX;
	[Attribute("", UIWidgets.EditBox, "Tool to enable dmg eg 'ToolAxe'")]
	string m_sToolName;
	
	DamageManagerComponent m_DamageManager;
	bool m_bAddedDamage;
	
	
	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		// Damage manager
		m_DamageManager = DamageManagerComponent.Cast(pOwnerEntity.FindComponent(DamageManagerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDamage(EDamageType type, float damage, HitZone pOriginalHitzone, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		//@matousvoj1 FIX PLS :(
		/*
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(instigator);
		if (!player)
		{
			Print("NO player dmg " + instigator, LogLevel.WARNING);
			return;
		}
		
		BaseWeaponManagerComponent charWeaponManager = player.GetCharacterController().GetWeaponManagerComponent();
		string sWeaponSlotType = charWeaponManager.GetCurrentWeapon().GetUIInfo().GetName();
				
		if (sWeaponSlotType != m_sToolName)
			return;
		*/
		if (damage != 66.543)
			return;
		
		if (IsProxy())
		{
			EL_Utils.SpawnEntityPrefab(m_HitVFX, hitTransform[0], hitTransform[1]);
			return;
		}
		
		
		
		//Prevent recursion
		if (m_bAddedDamage)
		{
			return;
			m_bAddedDamage = false;
		}
		
		m_bAddedDamage = true;
		if (this != pOriginalHitzone)
			return;
		


		// Add true damage
		m_DamageManager.HandleDamage(EDamageType.TRUE, damage, hitTransform, m_DamageManager.GetOwner(), m_DamageManager.GetDefaultHitZone(), instigator, null, -1, -1);
		
	}

};