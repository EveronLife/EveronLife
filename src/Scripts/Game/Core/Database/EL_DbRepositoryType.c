class EL_DbRepositoryType
{
	protected static ref map<typename, typename> m_Mapping;

	//------------------------------------------------------------------------------------------------
	static typename Get(typename entityType)
	{
		if (!m_Mapping) m_Mapping = new map<typename, typename>();

		typename result = m_Mapping.Get(entityType);
		if (!result)
		{
			string repositoryTypeStr = string.Format("EL_DbRepository<%1>", entityType.ToString());

			result = repositoryTypeStr.ToType();

			if (result)
			{
				// Save default implementation repository to cache
				m_Mapping.Set(entityType, result);
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbRepositoryType(typename repositoryType, typename entityType)
	{
		if (!m_Mapping) m_Mapping = new map<typename, typename>();

		typename expectedBase = string.Format("EL_DbRepository<%1>", entityType.ToString()).ToType();
		if (!repositoryType.IsInherited(expectedBase))
		{
			Debug.Error(string.Format("Failed to register '%1' as repository for '%2'. '%1' must inherit from '%3'.", repositoryType, entityType, expectedBase));
		}

		m_Mapping.Set(entityType, repositoryType);
	}
}
