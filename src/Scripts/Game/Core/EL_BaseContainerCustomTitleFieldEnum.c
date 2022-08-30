class EL_BaseContainerCustomTitleFieldEnum: BaseContainerCustomTitle
{
	string m_sPropertyName;
	typename m_tEnumType;

	//------------------------------------------------------------------------------------------------
	void EL_BaseContainerCustomTitleFieldEnum(string propertyName, typename enumType)
	{
		m_sPropertyName = propertyName;
		m_tEnumType = enumType;
	}

	//------------------------------------------------------------------------------------------------
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		int value;
		if (!source.Get(m_sPropertyName, value)) return false;
		title = typename.EnumToString(m_tEnumType, value);
		return true;
	}
}
