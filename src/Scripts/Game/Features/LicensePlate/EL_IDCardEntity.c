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
	
	protected ref Widget m_wPIPRoot;
	protected static bool s_bPIPIsEnabled;
	protected RTTextureWidget m_wRenderTargetTextureWidget;
	protected RenderTargetWidget m_wRenderTargetWidget;
};