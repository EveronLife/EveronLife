class EL_DestructibleResourceHitZone : SCR_HitZone
{
	//------------------------------------------------------------------------------------------------
	override void OnDamage(notnull BaseDamageContext damageContext)
	{
		super.OnDamage(damageContext);

		if (System.IsConsoleApp()) return;

		IEntity owner = GetOwner();
		EL_DestructibleResourceComponent destructibleResource = EL_Component<EL_DestructibleResourceComponent>.Find(owner);
		if (destructibleResource)
		{
			EL_DestructibleResourceComponentClass settings = EL_DestructibleResourceComponentClass.Cast(destructibleResource.GetComponentData(owner));
			EL_Utils.SpawnEntityPrefab(settings.m_rHitEffect, damageContext.hitPosition, damageContext.hitNormal.VectorToAngles(), false);
		}
	}

	//------------------------------------------------------------------------------------------------
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		EL_DestructibleResourceComponent destructibleResource = EL_Component<EL_DestructibleResourceComponent>.Find(GetOwner());
		if (destructibleResource)
		{
			EL_DestructibleResourceComponentClass settings = EL_DestructibleResourceComponentClass.Cast(destructibleResource.GetComponentData(destructibleResource.GetOwner()));
			ResourceName currentTool = EL_Utils.GetPrefabName(damageContext.damageSource);
			foreach (EL_ResourceDestructionTool tool : settings.m_aTools)
			{
				if (tool.m_rTool == currentTool)
					return tool.m_fHitDamage;
			}
		}

		return 0.0;
	}
}
