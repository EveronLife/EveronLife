[EL_ComponentSaveDataType(EL_HitZoneContainerComponentSaveData, HitZoneContainerComponent, "HitZoneContainer"), BaseContainerProps()]
class EL_HitZoneContainerComponentSaveData : EL_ComponentSaveDataBase
{
	ref array<ref EL_PersistentHitZone> m_aHitzones;

	override bool ReadFrom(GenericComponent worldEntityComponent)
	{
		m_aHitzones = new array<ref EL_PersistentHitZone>();
		
		array<HitZone> outHitZones();
		HitZoneContainerComponent.Cast(worldEntityComponent).GetAllHitZones(outHitZones);
		
		foreach(HitZone hitZone : outHitZones)
		{
			EL_PersistentHitZone persistentHitZone();
			persistentHitZone.m_sName = hitZone.GetName();
			persistentHitZone.m_fHealth = hitZone.GetHealth();
			m_aHitzones.Insert(persistentHitZone);
		}
		
		return true;
	}
	
	override bool ApplyTo(GenericComponent worldEntityComponent)
	{
		array<HitZone> outHitZones();
		HitZoneContainerComponent.Cast(worldEntityComponent).GetAllHitZones(outHitZones);
		
		bool tryIdxAcces = outHitZones.Count() >= m_aHitzones.Count();
		
		foreach(int idx, EL_PersistentHitZone persistentHitZone : m_aHitzones)
		{
			HitZone hitZone;
			
			// Assume same ordering as on save and see if that matches
			if(tryIdxAcces)
			{
				HitZone idxHitZone = outHitZones.Get(idx);
				
				if(idxHitZone.GetName() == persistentHitZone.m_sName) hitZone = idxHitZone;
			}
			
			// Iterate all hitzones to hopefully find the right one
			if(!hitZone)
			{
				foreach(HitZone findHitZone : outHitZones)
				{
					if(findHitZone.GetName() == persistentHitZone.m_sName)
					{
						hitZone = findHitZone;
						break;
					}
				}
			}
			
			if(!hitZone)
			{
				Debug.Error(string.Format("'%1' unable to find hitZone with name '%2'. Ignored.", worldEntityComponent, persistentHitZone.m_sName));
				continue;
			}
			
			hitZone.SetHealth(persistentHitZone.m_fHealth);
		}
		
		return true;
	}
	
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		if (!saveContext.IsValid()) return false;
		
		saveContext.WriteValue("dataLayoutVersion", 1);
		saveContext.WriteValue("m_aHitzones", m_aHitzones);
		
		return true;
	}
	
	protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		if (!loadContext.IsValid()) return false;
		
		int dataLayoutVersion;
		loadContext.ReadValue("dataLayoutVersion", dataLayoutVersion);
		loadContext.ReadValue("m_aHitzones", m_aHitzones);
		
		return true;
	}
}

class EL_PersistentHitZone
{
	string m_sName;
	float m_fHealth;
}
