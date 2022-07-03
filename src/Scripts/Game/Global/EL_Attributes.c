
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
	private typename m_EnumType;
	private string m_PropertyName;
	private string m_sFormat;
	
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
		string _title = typename.EnumToString(m_EnumType, enumValue);
		string firstLetter = _title[0];
		firstLetter.ToUpper();
		_title.Replace("_", " ");
		_title.ToLower();
		_title = firstLetter + _title.Substring(1, _title.Length() - 1);
		title = string.Format(m_sFormat, _title);
		return true;
	}
}