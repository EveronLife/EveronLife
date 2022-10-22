class EL_DbRepositoryType
{
	protected static ref map<typename, typename> s_mMapping;

	//------------------------------------------------------------------------------------------------
	static typename Get(typename entityType)
	{
		if (!s_mMapping) s_mMapping = new map<typename, typename>();

		typename result = s_mMapping.Get(entityType);
		if (!result)
		{
			string repositoryTypeStr = string.Format("EL_DbRepository<%1>", entityType.ToString());

			result = repositoryTypeStr.ToType();

			if (result)
			{
				// Save default implementation repository to cache
				s_mMapping.Set(entityType, result);
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbRepositoryType(typename repositoryType, typename entityType)
	{
		if (!s_mMapping) s_mMapping = new map<typename, typename>();

		typename expectedBase = string.Format("EL_DbRepository<%1>", entityType.ToString()).ToType();
		if (!repositoryType.IsInherited(expectedBase))
		{
			Debug.Error(string.Format("Failed to register '%1' as repository for '%2'. '%1' must inherit from '%3'.", repositoryType, entityType, expectedBase));
		}

		s_mMapping.Set(entityType, repositoryType);
	}
}
