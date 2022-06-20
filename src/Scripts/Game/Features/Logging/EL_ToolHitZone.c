class EL_ToolHitZone : ScriptedHitZone
{
	[Attribute("0", UIWidgets.EditBox, "Scale of received damage that will be passed to parent vehicle", "0 10 0.01")]
	private float m_f;
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
	/*!
	Called after damage multipliers and thresholds are applied to received impacts and damage is applied to hitzone.
	This is also called when transmitting the damage to parent hitzones!
	\param type Type of damage
	\param damage Amount of damage received
	\param pOriginalHitzone Original hitzone that got dealt damage, as this might be transmitted damage.
	\param instigator Damage source parent entity (soldier, vehicle, ...)
	\param hitTransform [hitPosition, hitDirection, hitNormal]
	\param speed Projectile speed in time of impact
	\param colliderID ID of the collider receiving damage
	\param nodeID ID of the node of the collider receiving damage
	*/

	override void OnDamage(EDamageType type, float damage, HitZone pOriginalHitzone, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		super.OnDamage(type, damage, pOriginalHitzone, instigator, hitTransform, speed, colliderID, nodeID);
		
		if (m_bAddedDamage)
			return;
		
		m_bAddedDamage = true;
		
		if (this != pOriginalHitzone)
			return;
		
		if (IsProxy())
			return;
		
		
		
		//Check if damage is from axe to tree
		if (type != EDamageType.MELEE || GetOwner().Type() != Tree)
			return;
		
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(instigator);
		if (!player)
			return;
		
		BaseWeaponManagerComponent charWeaponManager = player.GetCharacterController().GetWeaponManagerComponent();
		BaseWeaponComponent charWeaponComp = charWeaponManager.GetCurrentWeapon();
		
		//TODO add custom weapon type to weapons
		//if (charWeaponComp.GetWeaponType != EWeaponType.WT_AXE)
		//	return;
		
		// Add true damage
		m_DamageManager.HandleDamage(EDamageType.TRUE, damage, hitTransform, m_DamageManager.GetOwner(), m_DamageManager.GetDefaultHitZone(), instigator, null, -1, -1);
		
	}

};