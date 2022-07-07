class EL_EntityIdGenerator
{
	protected static int m_Sequence;
	
	static string Generate()
	{
		return string.Format("UNIQUE_%1", m_Sequence++);;
	}
}
