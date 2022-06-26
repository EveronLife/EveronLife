class EL_ToggleRoofLights : LightUserAction {
	protected EL_roofLightsComponent m_LightComp;
	protected AnimationPlayerComponent m_AnimComponent;
	protected SignalsManagerComponent m_KnobSignalsManagerComponent;
	protected SoundComponent m_SoundComponent;
	protected IEntity disk, knob;
	protected IEntity parent;
	ref protected RandomGenerator generator = new RandomGenerator();
	protected float m_PitchVariance = generator.RandFloatXY(0.9, 1.1);
	protected float m_VolumeVariance = generator.RandFloatXY(0.9, 1.1);
	
	protected void ToggleAnim(AnimationPlayerComponent anim, IEntity owner) 
	{
		if(anim.IsPlaying())
			anim.Stop(owner);
		
		else
			anim.Play(owner);
	}
	
	protected array<IEntity> FindPrefabs(IEntity owner)
	{
		IEntity nextEntity, mount;
		nextEntity = owner.GetChildren();
		while((!knob || !mount) && nextEntity)
		{
			Print(nextEntity);
			if(nextEntity.GetPrefabData() && nextEntity.GetPrefabData().GetPrefab().GetName().Contains("Mount"))
				mount = nextEntity;
			if(nextEntity.GetPrefabData() && nextEntity.GetPrefabData().GetPrefab().GetName().Contains("Knob"))
				knob = nextEntity;
			nextEntity = nextEntity.GetSibling();
			
		}
		nextEntity = mount;
		
		while (!m_LightComp && nextEntity)
		{
			disk = nextEntity;
			m_LightComp = EL_roofLightsComponent.Cast(nextEntity.FindComponent(EL_roofLightsComponent));
			nextEntity = nextEntity.GetChildren();
		} 
		m_AnimComponent = AnimationPlayerComponent.Cast(disk.FindComponent(AnimationPlayerComponent));
		m_SoundComponent = SoundComponent.Cast(owner.FindComponent(SoundComponent));
		m_KnobSignalsManagerComponent = SignalsManagerComponent.Cast(knob.FindComponent(SignalsManagerComponent));
		return {knob, disk};
	}
	
	protected void SetState();
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
    {	
				
		array<IEntity> knobParent;
		if(!m_LightComp || !m_SoundComponent)
		{
			knobParent = FindPrefabs(pOwnerEntity);
			knob = knobParent[0];
			disk = knobParent[1];
			m_AnimComponent = AnimationPlayerComponent.Cast(disk.FindComponent(AnimationPlayerComponent));
			m_SoundComponent = SoundComponent.Cast(pOwnerEntity.FindComponent(SoundComponent));
			m_KnobSignalsManagerComponent = SignalsManagerComponent.Cast(knob.FindComponent(SignalsManagerComponent));
			m_LightComp = EL_roofLightsComponent.Cast(disk.FindComponent(EL_roofLightsComponent));
			m_SoundComponent.SetSignalValueStr("PitchVariance", m_PitchVariance);
			m_SoundComponent.SetSignalValueStr("VolumeVariance", m_VolumeVariance);
		}
		
		SetState();
		
			
		
			
    }
};