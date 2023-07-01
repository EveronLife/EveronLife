class EL_DestructibleResourceHitZone : ScriptedHitZone
{
	//------------------------------------------------------------------------------------------------
	override void OnDamage(EDamageType type, float damage, HitZone pOriginalHitzone, IEntity instigator, inout vector hitTransform[3], float speed, int colliderID, int nodeID)
	{
		super.OnDamage(type, damage, pOriginalHitzone, instigator, hitTransform, speed, colliderID, nodeID);

		if (System.IsConsoleApp()) return;

		IEntity owner = GetOwner();
		EL_DestructibleResourceComponent destructibleResource = EL_Component<EL_DestructibleResourceComponent>.Find(owner);
		if (destructibleResource)
		{
			EL_DestructibleResourceComponentClass settings = EL_DestructibleResourceComponentClass.Cast(destructibleResource.GetComponentData(owner));
			EL_Utils.SpawnEntityPrefab(settings.m_rHitEffect, hitTransform[0], hitTransform[2], false);
		}
	}

	//------------------------------------------------------------------------------------------------
	override float ComputeEffectiveDamage(EDamageType damageType, float rawDamage, IEntity hitEntity, HitZone struckHitZone, IEntity damageSource, IEntity damageSourceGunner, IEntity damageSourceParent, const GameMaterial hitMaterial, int colliderID, inout vector hitTransform[3], const vector impactVelocity, int nodeID, bool isDOT)
	{
		EL_DestructibleResourceComponent destructibleResource = EL_Component<EL_DestructibleResourceComponent>.Find(GetOwner());
		if (destructibleResource)
		{
			EL_DestructibleResourceComponentClass settings = EL_DestructibleResourceComponentClass.Cast(destructibleResource.GetComponentData(destructibleResource.GetOwner()));
			ResourceName currentTool = EL_Utils.GetPrefabName(damageSource);
			foreach (EL_ResourceDestructionTool tool : settings.m_aTools)
			{
				if (tool.m_rTool == currentTool) return tool.m_fHitDamage;
			}
		}

		return 0.0;
	}
}
