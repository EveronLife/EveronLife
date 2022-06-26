class EL_roofLightsComponentClass : SCR_BaseInteractiveLightComponentClass
{
	
}

class EL_roofLightsComponent : SCR_BaseInteractiveLightComponent 
{
	[Attribute(uiwidget: UIWidgets.Auto)]
	protected ResourceName light_prefab;
	void ToggleLight(bool turnOn, bool skipTransition = false, bool playSound = true, IEntity parent = null)
	{
		if (!GetGame().InPlayMode())
			return;	
		
		SCR_BaseInteractiveLightComponentClass componentData = SCR_BaseInteractiveLightComponentClass.Cast(GetComponentData(GetOwner()));
		if (!componentData) 
			return;
		
		if (turnOn && !IsOn())
		{			
			TurnOn(componentData, skipTransition, playSound, parent);
		}
		else if(!turnOn)
		{
			TurnOff(componentData, playSound);
		}
	}
	
	protected void TurnOn(notnull SCR_BaseInteractiveLightComponentClass componentData, bool skipTransition, bool playSound, IEntity parent) 
	{
		
		if (!System.IsConsoleApp() && playSound && m_SoundComponent)
		{
			m_SoundComponent.SoundEvent("SOUND_TURN_ON");
		}

		LightEntity light;
		IEntity owner = GetOwner();
		vector lightOffset;
		vector lightDirection;
		vector pos;
		EntitySpawnParams params();
		
		
		vector mat[4], vec_debug;
		owner.GetLocalTransform(mat);
		params.Parent = parent;
		params.TransformMode = ETransformMode.LOCAL;
		
		lightOffset = componentData.GetLightData()[0].GetLightOffset();
		lightDirection = (componentData.GetLightData()[0].GetLightConeDirection().Multiply4(mat) - lightOffset.Multiply4(mat)).Normalized();
		pos = owner.GetOrigin() + lightOffset;
		params.Transform = mat;
		
		light = LightEntity.Cast(GetGame().SpawnEntityPrefabLocal(Resource.Load(light_prefab), owner.GetWorld(), params));
		
		light.CreateLight(LightType.SPOT, LightFlags.CASTSHADOW & LightFlags.DYNAMIC, 20.0, Color.FromRGBA(0, 0, 255, 255), 12.0, pos);
		light.SetConeAngle(150.0);
		
		m_aLights = {};
		
		m_aLights.Insert(light);
	
		if(light)
		{
			parent.AddChild(light, parent.GetBoneIndex("Disk"));
	
		}

		m_bIsOn = true;

		// Skip transition phase of the light.		
		if (skipTransition || !componentData.IsGradualLightningOn())
		{
			if (m_EmissiveMaterialComponent)
				m_EmissiveMaterialComponent.SetEmissiveMultiplier(MATERIAL_EMISSIVITY_ON);

			for (int i = 0, count = m_aLights.Count(); i < count; i++)
			{
				m_aLights[i].SetColor(Color.FromVector(componentData.GetLightData()[i].GetLightColor()), componentData.GetLightLV());
			}

			return;
		}
		
		m_fCurLV = LIGHT_EMISSIVITY_START;
		m_fCurEM = MATERIAL_EMISSIVITY_START;
		m_fLightEmisivityStep = componentData.GetLightLV() / ((MATERIAL_EMISSIVITY_ON - MATERIAL_EMISSIVITY_START) / MATERIAL_EMISSIVITY_STEP);
		m_fSoundSignalStep = 1 / ((MATERIAL_EMISSIVITY_ON - MATERIAL_EMISSIVITY_START) / MATERIAL_EMISSIVITY_STEP);
		m_fSoundSignalValue = 0;
		
		GetGame().GetCallqueue().CallLater(UpdateLight, UPDATE_LIGHT_TIME, true);
	}
}