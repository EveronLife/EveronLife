class Licence
{
	protected RTTextureWidget m_wRenderTargetTextureWidget;
	TextWidget licence_TextWidget;
	
	void Licence(string widgetname, string displayname, Widget wPIPRoot)
	{
		m_wRenderTargetTextureWidget = RTTextureWidget.Cast(wPIPRoot.FindAnyWidget("RTTexture0"));
		licence_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget(widgetname));	
		if(licence_TextWidget)
		{
			licence_TextWidget.SetText(displayname);
		}
	} 
}
