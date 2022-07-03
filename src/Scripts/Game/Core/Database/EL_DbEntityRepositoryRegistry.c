class EL_DbEntityRepositoryType
{
	void EL_DbEntityRepositoryType(typename entityType, typename repositoryType)
	{
		EL_DbEntityRepositoryRegistry.Register(entityType, repositoryType);
	}
}

class EL_DbEntityRepositoryRegistry
{
	protected static ref map<string, typename> m_Repositories;
	
	static void Register(typename entityType, typename repositoryType)
	{
		if(!m_Repositories)
		{
			m_Repositories = new map<string, typename>();
		}
		
		m_Repositories.Set(entityType.ToString(), repositoryType);
	}
	
	static typename Get(typename entityType)
	{
		typename result = typename.Empty;
		
		if(m_Repositories)
		{
			m_Repositories.Get(entityType.ToString())
		}
		
		if(!result)
		{
			string repositoryTypeStr = string.Format("EL_DbEntityRepository<%1>", entityType.ToString());
			
			result = repositoryTypeStr.ToType();
			
			if(!result)
			{
				Debug.Error(string.Format("Tried to get unknown entity repository type '%1'. Make sure you use it somewhere in your code e.g.: '%1 repository = ...;'", repositoryTypeStr));
			}
		}

		return result;
	}
	
	static void Reset()
	{
		delete m_Repositories;
	}
}
