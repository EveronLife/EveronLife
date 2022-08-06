class EL_DbEntityRepositoryType
{
	protected static ref map<typename, typename> m_Mapping;
	
	void EL_DbEntityRepositoryType(typename repositoryType, typename entityType)
	{
		if(!m_Mapping) m_Mapping = new map<typename, typename>();
		
		typename expectedBase = string.Format("EL_DbEntityRepository<%1>", entityType.ToString()).ToType();
		
		if(!repositoryType.IsInherited(expectedBase))
		{
			Debug.Error(string.Format("Failed to register '%1' as repository for '%2'. '%1' must inherit from '%3'.", repositoryType, entityType, expectedBase));
		}
		
		m_Mapping.Set(entityType, repositoryType);
	}
	
	static typename Get(typename entityType)
	{
		if(!m_Mapping) m_Mapping = new map<typename, typename>();
		
		typename result = m_Mapping.Get(entityType);
		
		if(!result)
		{
			string repositoryTypeStr = string.Format("EL_DbEntityRepository<%1>", entityType.ToString());
			
			result = repositoryTypeStr.ToType();
			
			if(result)
			{
				// Save default implementation repository to cache
				m_Mapping.Set(entityType, result);
			}
			else
			{
				Debug.Error(string.Format("Tried to get unknown entity repository type '%1'. Make sure you use it somewhere in your code e.g.: '%1 repository = ...;'", repositoryTypeStr));
			}
		}
		
		return result;
	}
}
