class EL_ToolHitZone : ScriptedHitZone
{

	[Attribute("", UIWidgets.ResourceNamePicker, "Particle to spawn on hit", "et")]
	ResourceName m_HitVFX;
	[Attribute("", UIWidgets.Slider, "Damage reference to the effective tool", "0.0 100.0 1.0")]
	float m_fEffectiveDamage;

	//------------------------------------------------------------------------------------------------
	override void OnDamage(EDamageType type, float damage, HitZone pOriginalHitzone, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		EL_Utils.SpawnEntityPrefab(m_HitVFX, hitTransform[0], hitTransform[1]);
		
		if (!instigator) return;
		
		//Heal any damage that is not the effective tool (dmg)
		if (damage != m_fEffectiveDamage)
			HandleDamage(-damage, EDamageType.TRUE, instigator);
		
		
	}

	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
	}

};