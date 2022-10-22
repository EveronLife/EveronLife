class EL_WhitelistManager {

	private static EL_DbContext m_pDatabase;

	//------------------------------------------------------------------------------------------------
	static bool ValidUuid(string uuid)
	{
		return uuid && uuid.Length() >= 32;
	}

	//------------------------------------------------------------------------------------------------
	static EL_WhitelistDbEntity GetWhitelistEntity(string uuid)
	{
		if (!Replication.IsServer())
		{
			return null;
		}

		if(!m_pDatabase)
		{
			m_pDatabase = EL_DbContextFactory.GetContext();
		}

		if(!ValidUuid(uuid))
		{
			return null;
		}

		EL_DbFindCondition condition = EL_DbFind.Field("m_sUuid").Equals(uuid);
		EL_DbFindResults<EL_DbEntity> query = m_pDatabase.FindAll(EL_WhitelistDbEntity, condition);

		if(!query.Success())
		{
			return null;
		}

		if(query.GetEntities().Count() != 1 && query.GetEntities().Get(0))
		{
			return null;
		}

		return EL_WhitelistDbEntity.Cast(query.GetEntities().Get(0));
	}

	//------------------------------------------------------------------------------------------------
	static bool UuidExistInWhitelist(string uuid)
	{
		if (!Replication.IsServer())
		{
			return false;
		}

		if(!ValidUuid(uuid))
		{
			return false;
		}

		return GetWhitelistEntity(uuid) != null;
	}

	//------------------------------------------------------------------------------------------------
	static bool UpdateAndSaveWhitelistEntity(EL_WhitelistDbEntity userInfo)
	{
		if (!Replication.IsServer())
		{
			return false;
		}

		if(!m_pDatabase)
		{
			m_pDatabase = EL_DbContextFactory.GetContext();
		}

		System.GetYearMonthDay(userInfo.m_iYear, userInfo.m_iMonth, userInfo.m_iDay);
		System.GetHourMinuteSecond(userInfo.m_iHour, userInfo.m_iMinute, userInfo.m_iSecond);

		EL_EDbOperationStatusCode code = m_pDatabase.AddOrUpdate(userInfo);

		return EL_EDbOperationStatusCode.SUCCESS == code;
	}

	//------------------------------------------------------------------------------------------------
	static bool UpdateAndSaveWhitelistEntity(string uuid)
	{
		if (!Replication.IsServer())
		{
			return false;
		}

		if(!ValidUuid(uuid))
		{
			return false;
		}

		EL_WhitelistDbEntity whitelistEntity = GetWhitelistEntity(uuid);

		if(whitelistEntity == null)
		{
			return false;
		}

		return UpdateAndSaveWhitelistEntity(whitelistEntity);
	}

	//------------------------------------------------------------------------------------------------
	static array<string> GetAllUuidFromFile()
	{
		if (!Replication.IsServer())
		{
			return null;
		}

		bool fileExist = FileIO.FileExist("$profile:whitelist.txt");

		if(!fileExist)
		{
			return null;
		}

		FileHandle uuids = FileIO.OpenFile("$profile:whitelist.txt", FileMode.READ);
		string uuid;

		if(uuids == 0)
		{
			return null;
		}

		array<string> validUuid();

		while(uuids.FGets(uuid) > 0)
		{
			if(ValidUuid(uuid))
			{
				validUuid.Insert(uuid);
			}
		}

		uuids.CloseFile();

		return validUuid;
	}

	//------------------------------------------------------------------------------------------------
	static array<ref EL_WhitelistDbEntity> GetAllWhitelistFromDatabase()
	{
		if (!Replication.IsServer())
		{
			return null;
		}

		if(!m_pDatabase)
		{
			m_pDatabase = EL_DbContextFactory.GetContext();
		}

		EL_DbFindResults<EL_DbEntity> query = m_pDatabase.FindAll(EL_WhitelistDbEntity);

		if(!query.Success())
		{
			return null;
		}

		return EL_RefArrayCaster<EL_DbEntity, EL_WhitelistDbEntity>.Convert(query.GetEntities());
	}

	//------------------------------------------------------------------------------------------------
	static array<string> GetAllUuidInDatabase()
	{
		if (!Replication.IsServer())
		{
			return null;
		}

		array<string> uuids();

		foreach(EL_WhitelistDbEntity wh : GetAllWhitelistFromDatabase())
		{
			if(wh.m_sUuid && ValidUuid(wh.m_sUuid))
			{
				uuids.Insert(wh.m_sUuid);
			}
		}

		return uuids;
	}

	//------------------------------------------------------------------------------------------------
	static EL_WhitelistDbEntity NewEmptyWhitelistEntity()
	{
		EL_WhitelistDbEntity newUser();

		newUser.m_sUuid = string.Empty;
		newUser.m_iYear = 0;
		newUser.m_iMonth = 0;
		newUser.m_iDay = 0;
		newUser.m_iHour = 0;
		newUser.m_iMinute = 0;
		newUser.m_iSecond = 0;

		return newUser;
	}

	//------------------------------------------------------------------------------------------------
	static EL_WhitelistDbEntity NewEmptyWhitelistEntity(string uuid)
	{
		EL_WhitelistDbEntity newUser = NewEmptyWhitelistEntity();
		newUser.m_sUuid = uuid;

		return newUser;
	}

	//------------------------------------------------------------------------------------------------
	static bool AddNewUuidToWhiteList(string uuid)
	{
		if (!Replication.IsServer())
		{
			return false;
		}

		if(!ValidUuid(uuid))
		{
			return false;
		}

		if(UuidExistInWhitelist(uuid))
		{
			return false;
		}

		if(!m_pDatabase)
		{
			m_pDatabase = EL_DbContextFactory.GetContext();
		}

		EL_WhitelistDbEntity user = NewEmptyWhitelistEntity(uuid);

		EL_EDbOperationStatusCode code = m_pDatabase.AddOrUpdate(user);

		return EL_EDbOperationStatusCode.SUCCESS == code;
	}
}