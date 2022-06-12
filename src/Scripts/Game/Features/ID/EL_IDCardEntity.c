[EntityEditorProps(category: "EL/IDCardEntity", description:"The ID Card Entity")]
class EL_IDCardEntityClass : GenericEntityClass
{
};

class EL_IDCardEntity : GenericEntity
{
	protected Widget m_RootWidget;
	
	// Attribute for the layout
	[Attribute("{EF091399D840192D}UI/layouts/Sights/PictureInPictureSightsLayout.layout", UIWidgets.ResourcePickerThumbnail, "The layout used for the PIP component", params: "layout")]
	protected ResourceName m_Layout;
	
	// Attribute to set the firstname (Just for testing purpose)
	[Attribute("Firstname", UIWidgets.EditBox, "Surname")]
	protected string surname_blank;
	
	// Attribute to set the lastname (Just for testing purpose)
	[Attribute("Lastname", UIWidgets.EditBox, "Lastname")]
	protected string lastname_blank;
	
	// Attribute to set the birthday (Just for testing purpose)
	[Attribute("Birthday", UIWidgets.EditBox, "Birthday")]
	protected string birthday_blank;
	
	// Attribute to set the licence1 (Just for testing purpose)
	[Attribute("Licence1", UIWidgets.EditBox, "Licence1")]
	protected string licence1_blank;
	
	// Attribute to set the licence2 (Just for testing purpose)
	[Attribute("Licence2", UIWidgets.EditBox, "Licence2")]
	protected string licence2_blank;
	
	// Attribute to set the licence3 (Just for testing purpose)
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
	
	//Plays the code every frame.
	protected override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_wPIPRoot)
		{
			//Gets the Layout
			m_wPIPRoot = GetGame().GetWorkspace().CreateWidgets(m_Layout);
	
			//If there is no layout it returns
			if (!m_wPIPRoot) return;
			
			//Casts the RTTexture
			m_wRenderTargetTextureWidget = RTTextureWidget.Cast(m_wPIPRoot.FindAnyWidget("RTTexture0"));
			
			if (!m_wRenderTargetTextureWidget)
			{
				m_wPIPRoot.RemoveFromHierarchy();
				m_wPIPRoot = null;
				return;
			}
			//Casts the Information from the layout.
			surname_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Surname"));
			lastname_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Lastname"));
			birthday_TextWidget = TextWidget.Cast(m_wRenderTargetTextureWidget.FindAnyWidget("Birthday"));
			
			//Creats the licences & casts it on the ID Card. 
			//This creation is later for processers that can check if a player has an IDCard with the right licenses
			//If wished to create more licences then 3, go in the layout, add "licence4, etc" and then add it here.
			Licence licence = new Licence("Licence1", "Driving Licence", m_wPIPRoot);
			Licence licence2 = new Licence("Licence2", "Food Licence", m_wPIPRoot);
			
			if ( surname_TextWidget && lastname_TextWidget && birthday_TextWidget)
			{	
				//Casts the Information about user.
				surname_TextWidget.SetText(surname_blank);
				lastname_TextWidget.SetText(lastname_blank);
				birthday_TextWidget.SetText(birthday_blank);
			}
			
		}
		
		m_wRenderTargetTextureWidget.SetGUIWidget(this, m_MaterialIndex);
	}
	
}