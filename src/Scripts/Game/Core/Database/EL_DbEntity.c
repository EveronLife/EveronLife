class EL_DbEntity
{
	static const string FIELD_ID = "m_sId";

	private string m_sId;

	//------------------------------------------------------------------------------------------------
	//! Get the unique id of the db entity. Guaranteed to remain a string in any future version
	string GetId()
	{
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Set the unique id on the db entity. Must always be a string!
	void SetId(string id)
	{
		m_sId = id;
	}

	//------------------------------------------------------------------------------------------------
	//! Check if an id has been assigned
	bool HasId()
	{
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Utility function to write id to serializer
	void WriteId(notnull BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue(FIELD_ID, m_sId);
	}

	//------------------------------------------------------------------------------------------------
	//! Utility function to read id from serializer
	void ReadId(notnull BaseSerializationLoadContext loadContext)
	{
		loadContext.ReadValue(FIELD_ID, m_sId);
	}
};

class EL_DbName
{
	protected static const string AUTO_GENERATE = "AUTO_GENERATE";

	protected static ref map<typename, string> s_mMapping;
	protected static ref map<string, typename> s_mReverseMapping;

	//------------------------------------------------------------------------------------------------
	static string Get(typename entityType)
	{
		if (s_mMapping)
			return s_mMapping.Get(entityType);

		return entityType.ToString();
	}

	//------------------------------------------------------------------------------------------------
	static typename GetTypeByName(string name)
	{
		if (s_mReverseMapping)
			return s_mReverseMapping.Get(name);

		return name.ToType();
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbName Automatic()
	{
		return new EL_DbName(AUTO_GENERATE);
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbName(string name = AUTO_GENERATE)
	{
		typename entityType = EL_ReflectionUtils.GetAttributeParent();

		if (!s_mMapping)
			s_mMapping = new map<typename, string>();

		if (!s_mReverseMapping)
			s_mReverseMapping = new map<string, typename>();

		if (name == AUTO_GENERATE)
		{
			name = entityType.ToString();

			int tagIdx = name.IndexOf("_");
			if (tagIdx != -1 && (tagIdx + 1) < name.Length())
				name = name.Substring(tagIdx + 1, name.Length() - (tagIdx + 1));

			if (name.StartsWith("Base"))
				name = name.Substring(4, name.Length() - 4);

			if (name.EndsWith("SaveData"))
				name = name.Substring(0, name.Length() - 8);

			s_mMapping.Set(entityType, name);
			s_mReverseMapping.Set(name, entityType);
		}

		s_mMapping.Set(entityType, name);
		s_mReverseMapping.Set(name, entityType);
	}
};
