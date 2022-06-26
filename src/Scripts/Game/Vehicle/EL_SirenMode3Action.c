class SirenMode3Action : EL_ToggleRoofLights {
	override protected void SetState(){

		m_LightComp.ToggleLight(true, true, false, disk);
		if (m_SoundComponent)
		{
			m_SoundComponent.SetSignalValueStr("Mode0", 0);
			m_SoundComponent.SetSignalValueStr("Mode1", 0);
			m_SoundComponent.SetSignalValueStr("Mode2", 0);
			m_SoundComponent.SetSignalValueStr("Mode3", 1);
			
		}
		
		m_KnobSignalsManagerComponent.SetSignalValue(m_KnobSignalsManagerComponent.FindSignal("Translation"), -0.01);
			
		
	
		if(m_AnimComponent && !m_AnimComponent.IsPlaying())
			m_AnimComponent.Play(disk);
	}
}