class EL_ToolHitZone : ScriptedHitZone
{

	[Attribute("", UIWidgets.ResourceNamePicker, "Particle to spawn on hit", "et")]
	ResourceName m_HitVFX;
	[Attribute("", UIWidgets.Slider, "Damage reference to the effective tool", "0.0 100.0 1.0")]
	float m_fEffectiveDamage;

	//------------------------------------------------------------------------------------------------
	override void OnLocalDamage(EDamageType type, float damage, HitZone pOriginalHitzone, IEntity damageSource, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		super.OnLocalDamage(type, damage, pOriginalHitzone, damageSource, instigator, hitTransform, speed, colliderID, nodeID);
		EL_Utils.SpawnEntityPrefab(m_HitVFX, hitTransform[0], hitTransform[1]);	
	}

};