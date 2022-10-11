[ComponentEditorProps(category: "EveronLife/Feature/Whitelist", description: "Allows to thread the users who connect to the server.")]
class EL_WhitelistComponentClass: SCR_RespawnHandlerComponentClass
{
}

class EL_WhitelistComponent : SCR_RespawnHandlerComponent
{
	private EL_DbContext m_pDatabase;
	private bool m_bWhitelistEnable;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (RplSession.Mode() == RplMode.Dedicated)
		{
			return;
		}

		if(!m_pDatabase)
		{
			m_pDatabase = EL_DbContextFactory.GetContext();
		}

		int uuidCreated = 0;
		int uuidRemove = 0;
		int uuidReadFile = 0;
		int uuidReadDb = 0;

		array<string> uuids = EL_WhitelistManager.GetAllUuidFromFile();

		if(uuids.Count() == 0)
		{
			// TODO Use Setting !
			EL_Utils.Logger("EL_WhitelistHandlerComponent", "OnPostInit", "Whitelist not activated", LogLevel.NORMAL);
			m_bWhitelistEnable = false;
			return;
		}

		uuidReadFile = uuids.Count();

		array<ref EL_WhitelistDbEntity> allBddEntity = EL_WhitelistManager.GetAllWhitelistFromDatabase();
		array<string> allBddUuid = EL_WhitelistManager.GetAllUuidInDatabase();

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

		string message = string.Format("UUID Read : (File : %1 : Database : %2) | UUID add to list: %3 | UUID remove from list : %4", uuidReadFile, uuidReadDb, uuidCreated, uuidRemove);
		EL_Utils.Logger("EL_WhitelistHandlerComponent", "OnPostInit", message, LogLevel.DEBUG);

		m_bWhitelistEnable = true;
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerConnected(int playerId)
	{
		if(!m_bWhitelistEnable)
		{
			return;
		}

		if (RplSession.Mode() == RplMode.Dedicated)
		{
			return;
		}

		string uuid = EL_Utils.GetPlayerUID(playerId);

		if(!EL_WhitelistManager.UuidExistInWhitelist(uuid))
		{
			string message = string.Format("Attempt to connect an unauthorized user (uuid : %1)", uuid);
			EL_Utils.Logger("EL_WhitelistHandlerComponent", "OnPlayerConnected", message, LogLevel.WARNING, true);
			BackToMainMenu();
			return;
		}

		EL_WhitelistManager.UpdateAndSaveWhitelistEntity(uuid);
	}

	//------------------------------------------------------------------------------------------------
	private void BackToMainMenu()
	{
		#ifdef WORKBENCH

		EL_Utils.Logger("EL_WhitelistHandlerComponent", "BackToMainMenu", "You are not in the whitelist, but we are a WORKBENCH.", LogLevel.FATAL, true);
		return;

		#endif

		GameStateTransitions.RequestGameplayEndTransition();
	}
}
