/**
	\brief Attribute for setting any enum property as custom title.
	@code
	[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EEditorMode, "m_Mode")]
	class TestConfigClass
	{
		[Attribute()]
		private EEditorMode m_Mode;
	}
	@endcode
*/
class EL_BaseContainerCustomTitleEnumReadable : BaseContainerCustomTitle
{
	protected typename m_EnumType;
	protected string m_PropertyName;
	protected string m_sFormat;
	
	void EL_BaseContainerCustomTitleEnumReadable(typename enumType, string propertyName, string format = "%1")
	{
		m_EnumType = enumType;
		m_PropertyName = propertyName;
		m_sFormat = format;
	}
	
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		int enumValue;
		if (!source.Get(m_PropertyName, enumValue))
		{
			return false;
		}
		string titleValue = typename.EnumToString(m_EnumType, enumValue);
		string firstLetter = titleValue[0];
		firstLetter.ToUpper();
		titleValue.Replace("_", " ");
		titleValue.ToLower();
		titleValue = firstLetter + titleValue.Substring(1, titleValue.Length() - 1);
		title = string.Format(m_sFormat, titleValue);
		return true;
	}
}
