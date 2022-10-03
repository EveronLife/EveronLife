class EL_ToolHitZone : ScriptedHitZone
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Particle to spawn on hit", "et")]
	ResourceName m_HitVFX;
	[Attribute("", UIWidgets.Slider, "Effective tool damage", "0.0 100.0 0.01")]
	float m_fEffectiveDamage;

	//------------------------------------------------------------------------------------------------
	override void OnDamage(EDamageType type, float damage, HitZone pOriginalHitzone, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		//Melee damage reduction is 0.01
		if (damage == m_fEffectiveDamage * 0.01)
			HandleDamage(damage * 100, EDamageType.TRUE, instigator);
	}
	//------------------------------------------------------------------------------------------------
	override void OnLocalDamage(EDamageType type, float damage, HitZone pOriginalHitzone, IEntity damageSource, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		super.OnLocalDamage(type, damage, pOriginalHitzone, damageSource, instigator, hitTransform, speed, colliderID, nodeID);

		if (damage == m_fEffectiveDamage * 0.01)
			EL_Utils.SpawnEntityPrefab(m_HitVFX, hitTransform[0], hitTransform[1]);
	}
};
