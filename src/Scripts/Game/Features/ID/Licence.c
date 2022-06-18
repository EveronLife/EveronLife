class Licence
{
	protected RTTextureWidget m_wRenderTargetTextureWidget;
	TextWidget licenceTextWidget;
	
	void Licence(string widgetname, string displayname, Widget m_wPIPRoot)
	{
		m_wRenderTargetTextureWidget = RTTextureWidget.Cast(wPIPRoot.FindAnyWidget("RTTexture0"));
		licenceTextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget(widgetname));	
		if(licenceTextWidget)
		{
			licenceTextWidget.SetText(displayname);
		}
	} 
}