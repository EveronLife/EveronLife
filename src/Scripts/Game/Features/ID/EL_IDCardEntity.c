[EntityEditorProps(category: "EL/IDCardEntity", description:"The ID Card Entity")]
class EL_IDCardEntityClass : GenericEntityClass
{
};

class EL_IDCardEntity : GenericEntity
{
	protected Widget m_RootWidget;
	
	[Attribute("{EF091399D840192D}UI/layouts/Sights/PictureInPictureSightsLayout.layout", UIWidgets.ResourcePickerThumbnail, "The layout used for the PIP component", params: "layout")]
	protected ResourceName m_Layout;
	
	[Attribute("2", UIWidgets.Slider, "Material Index", params: "0 10 1")]
	protected int m_MaterialIndex;
	
	[Attribute("Firstname", UIWidgets.EditBox, "Surname")]
	protected string surname_blank;
	
	[Attribute("Lastname", UIWidgets.EditBox, "Lastname")]
	protected string lastname_blank;
	
	[Attribute("Birthday", UIWidgets.EditBox, "Birthday")]
	protected string birthday_blank;
	
	[Attribute("Licence1", UIWidgets.EditBox, "Licence1")]
	protected string licence1_blank;
	
	[Attribute("Licence2", UIWidgets.EditBox, "Licence2")]
	protected string licence2_blank;
	
	[Attribute("Licence2", UIWidgets.EditBox, "Licence3")]
	protected string licence3_blank;
	
	protected ref Widget m_wPIPRoot;
	protected static bool s_bPIPIsEnabled;
	protected RTTextureWidget m_wRenderTargetTextureWidget;
	protected RenderTargetWidget m_wRenderTargetWidget;
	protected RplComponent m_RplComponent;
	
	TextWidget surname_TextWidget;
	TextWidget lastname_TextWidget;
	TextWidget birthday_TextWidget;
	TextWidget licence1_TextWidget;
	TextWidget licence2_TextWidget;
	TextWidget licence3_TextWidget;
	
	void EL_IDCardEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.FRAME);
		SetFlags(EntityFlags.ACTIVE, false);
	}
	
	void ~EL_IDCardEntity()
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
			
			surname_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Surname"));
			lastname_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Lastname"));
			birthday_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Birthday"));
			licence1_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Licence1"));
			licence2_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Licence2"));
			licence3_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Licence3"));

			if ( surname_TextWidget && lastname_TextWidget && birthday_TextWidget)
			{	
				surname_TextWidget.SetText(surname_blank);
				lastname_TextWidget.SetText(lastname_blank);
				birthday_TextWidget.SetText(birthday_blank);
				licence1_TextWidget.SetText(licence1_blank);
				licence2_TextWidget.SetText(licence2_blank);
				licence3_TextWidget.SetText(licence3_blank);
			}
			
		}
		
		m_wRenderTargetTextureWidget.SetGUIWidget(this, m_MaterialIndex);
		Print(owner);
	}
	
};