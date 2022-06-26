class SirenMode0Action : EL_ToggleRoofLights {
	override protected void SetState(){
		
		Print("Action 0");
		m_LightComp.ToggleLight(false, true, false, disk);
		if (m_SoundComponent)
		{
			m_SoundComponent.SetSignalValueStr("Mode0", 1);
			m_SoundComponent.SetSignalValueStr("Mode1", 0);
			m_SoundComponent.SetSignalValueStr("Mode2", 0);
			m_SoundComponent.SetSignalValueStr("Mode3", 0);
			
		}
		
		m_KnobSignalsManagerComponent.SetSignalValue(m_KnobSignalsManagerComponent.FindSignal("Rotation"), 0.0);
		m_KnobSignalsManagerComponent.SetSignalValue(m_KnobSignalsManagerComponent.FindSignal("Translation"), 0);
			
		
	
		if(m_AnimComponent && m_AnimComponent.IsPlaying())
			 m_AnimComponent.Stop(disk);
	}
}