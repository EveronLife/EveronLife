[ComponentEditorProps(category: "EveronLife/Feature/Whitelist", description: "Allows to thread the users who connect to the server.")]
class EL_WhitelistComponentClass: SCR_RespawnHandlerComponentClass
{
}

class EL_WhitelistComponent : SCR_RespawnHandlerComponent
{
	private EL_DbContext m_pDatabase;
	private bool m_bWhitelistInit;
	private bool m_bWhitelistEnable;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (!Replication.IsServer())
		{
			return;
		}

		m_bWhitelistInit = InitWhitelist();

		if (!m_bWhitelistInit)
		{
			EL_Utils.Logger("EL_WhitelistHandlerComponent", "OnPlayerDequeued", "Unable to initialize the whitelist, A new attempt will be executed at the first connection of a player", LogLevel.WARNING, true);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerDequeued(int playerId)
	{
		if (!Replication.IsServer())
		{
			return;
		}

		if (!m_bWhitelistInit)
		{
			m_bWhitelistInit = InitWhitelist();
		}

		if (!m_bWhitelistInit)
		{
			EL_Utils.Logger("EL_WhitelistHandlerComponent", "OnPlayerDequeued", "Unable to initialize the whitelist ?!", LogLevel.FATAL, true);
			return;
		}

		if (!m_bWhitelistEnable)
		{
			return;
		}

		bool valid = VerifyPlayer(playerId);

		if (!valid)
		{
			GetGame().GetPlayerManager().KickPlayer(playerId, PlayerManagerKickReason.KICK, 10);
		}
	}

	//------------------------------------------------------------------------------------------------
	private bool InitWhitelist()
	{
		if (!Replication.IsServer())
		{
			return false;
		}

		if(!m_pDatabase)
		{
			m_pDatabase = EL_DbContextFactory.GetContext();
		}

		if(!m_pDatabase)
		{
			return false;
		}

		int uuidCreated = 0;
		int uuidRemove = 0;
		int uuidReadFile = 0;
		int uuidReadDb = 0;

		array<string> uuids = EL_WhitelistManager.GetAllUuidFromFile();

		if(uuids == null || uuids.Count() == 0)
		{
			// TODO Use Setting !
			EL_Utils.Logger("EL_WhitelistHandlerComponent", "OnPostInit", "Whitelist not activated", LogLevel.NORMAL);
			m_bWhitelistEnable = false;
			return true;
		}

		uuidReadFile = uuids.Count();

		array<ref EL_WhitelistDbEntity> allBddEntity = EL_WhitelistManager.GetAllWhitelistFromDatabase();
		array<string> allBddUuid = EL_WhitelistManager.GetAllUuidInDatabase();

		if (allBddEntity == null || allBddUuid == null)
		{
			return false;
		}

		uuidReadDb = allBddUuid.Count();

		foreach(EL_WhitelistDbEntity wh : allBddEntity)
		{
			if (uuids.Contains(wh.m_sUuid))
			{
				continue;
			}

			EL_EDbOperationStatusCode code = m_pDatabase.Remove(wh);

			if(EL_EDbOperationStatusCode.SUCCESS == code)
			{
				uuidRemove++;
			}
		}

		foreach(string uuid : uuids)
		{
			if (!allBddUuid.Contains(uuid))
			{
				if(EL_WhitelistManager.AddNewUuidToWhiteList(uuid))
				{
					uuidCreated++;
				}
			}
		}

		string message = string.Format("UUID Read : (File : %1 : Database : %2) | UUID add to Database : %3 | UUID remove from Database : %4", uuidReadFile, uuidReadDb, uuidCreated, uuidRemove);
		EL_Utils.Logger("EL_WhitelistHandlerComponent", "OnPostInit", message, LogLevel.NORMAL);

		m_bWhitelistEnable = true;
		return true;
	}

	//------------------------------------------------------------------------------------------------
	private bool VerifyPlayer(int playerId)
	{
		if (!Replication.IsServer())
		{
			return false;
		}

		string uuid = EL_Utils.GetPlayerUID(playerId);

		if(!EL_WhitelistManager.UuidExistInWhitelist(uuid))
		{
			string message = string.Format("Attempt to connect an unauthorized user (uuid : %1)", uuid);
			EL_Utils.Logger("EL_WhitelistHandlerComponent", "OnPlayerConnected", message, LogLevel.WARNING, true);
			return false;
		}

		EL_WhitelistManager.UpdateAndSaveWhitelistEntity(uuid);

		return true;
	}
}
