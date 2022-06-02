[EntityEditorProps(category: "EL/LicensePlateEntity", description:"The License Plate Entity")]
class EL_LicensePlateEntityClass : GenericEntityClass
{
};

class EL_LicensePlateEntity : GenericEntity
{
	protected Widget m_RootWidget;
	
	[Attribute("{EF091399D840192D}UI/layouts/Sights/PictureInPictureSightsLayout.layout", UIWidgets.ResourcePickerThumbnail, "The layout used for the PIP component", params: "layout")]
	protected ResourceName m_Layout;
	
	[Attribute("2", UIWidgets.Slider, "Material Index", params: "0 10 1")]
	protected int m_MaterialIndex;
	
	protected ref Widget m_wPIPRoot;
	protected static bool s_bPIPIsEnabled;
	protected RTTextureWidget m_wRenderTargetTextureWidget;
	protected RenderTargetWidget m_wRenderTargetWidget;
	
	TextWidget m_TextWidget;
	EL_LicensePlateManagerComponent m_LicensePlateManager;
	
	void EL_LicensePlateEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.FRAME);
		SetFlags(EntityFlags.ACTIVE, false);
	}
	
	void ~EL_LicensePlateEntity()
	{
		if (m_wPIPRoot)
		{
			m_wPIPRoot.RemoveFromHierarchy();
			m_wPIPRoot = null;
		}
	}
	
	protected override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_wPIPRoot)
		{
			m_wPIPRoot = GetGame().GetWorkspace().CreateWidgets(m_Layout);
	
			if (!m_wPIPRoot) return;
			
			m_wRenderTargetTextureWidget = RTTextureWidget.Cast(m_wPIPRoot.FindAnyWidget("RTTexture0"));
			
			if (!m_wRenderTargetTextureWidget)
			{
				m_wPIPRoot.RemoveFromHierarchy();
				m_wPIPRoot = null;
				return;
			}
			
			m_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Text0"));
			
			if (m_LicensePlateManager && m_TextWidget)
			{
				m_TextWidget.SetText(m_LicensePlateManager.m_Registration);
			}
		}
		
		m_wRenderTargetTextureWidget.SetGUIWidget(this, m_MaterialIndex);
	}
};
