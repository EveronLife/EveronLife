class SirenMode1Action : EL_ToggleRoofLights {
	override protected void SetState(){
		Print("Action 1");
		m_LightComp.ToggleLight(true, true, false, disk);
		Print("After toggling");
		if (m_SoundComponent)
		{
			m_SoundComponent.SetSignalValueStr("Mode0", 0);
			m_SoundComponent.SetSignalValueStr("Mode1", 1);
			m_SoundComponent.SetSignalValueStr("Mode2", 0);
			m_SoundComponent.SetSignalValueStr("Mode3", 0);
			
		}
		
		m_KnobSignalsManagerComponent.SetSignalValue(m_KnobSignalsManagerComponent.FindSignal("Rotation"), 30.0);
		m_KnobSignalsManagerComponent.SetSignalValue(m_KnobSignalsManagerComponent.FindSignal("Translation"), 0);
			
		
	
		if(m_AnimComponent && !m_AnimComponent.IsPlaying())
			m_AnimComponent.Play(disk);
	}
}