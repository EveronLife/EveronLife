class EL_ToolHitZone : ScriptedHitZone
{

	[Attribute("", UIWidgets.ResourceNamePicker, "Particle to spawn on hit", "et")]
	ResourceName m_HitVFX;
	[Attribute("", UIWidgets.EditBox, "Tool to enable dmg eg 'ToolAxe'")]
	string m_sToolName;

	DamageManagerComponent m_DamageManager;
	bool m_bAddedDamage;



	//------------------------------------------------------------------------------------------------
	override void OnDamage(EDamageType type, float damage, HitZone pOriginalHitzone, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		//Axe check
		if (damage != 20.22)
			return;

		EL_Utils.SpawnEntityPrefab(m_HitVFX, hitTransform[0], hitTransform[1]);

		HandleDamage(damage * 2, EDamageType.MELEE, instigator);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_DamageManager = DamageManagerComponent.Cast(pOwnerEntity.FindComponent(DamageManagerComponent));
	}

};