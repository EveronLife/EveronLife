class EL_BufferedDbContext : EL_DbContext
{
	//------------------------------------------------------------------------------------------------
	void Flush(int maxBatchSize = 50)
	{
		EL_DbDriverBufferWrapper.Cast(m_Driver).Flush(maxBatchSize);
	}

	//------------------------------------------------------------------------------------------------
	override static EL_BufferedDbContext Create(notnull EL_DbConnectionInfoBase connectionInfo)
	{
		EL_DbContext baseContext = EL_DbContext.Create(connectionInfo);
		if (baseContext)
			return new EL_BufferedDbContext(baseContext.m_Driver);

		return null;
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_BufferedDbContext(notnull EL_DbDriver driver)
	{
		m_Driver = new EL_DbDriverBufferWrapper(driver);
	}
};
