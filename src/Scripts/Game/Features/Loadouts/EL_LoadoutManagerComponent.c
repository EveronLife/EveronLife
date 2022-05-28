class EL_LoadoutManagerComponentClass: ScriptComponentClass
{
}

class EL_LoadoutManagerComponent : ScriptComponent
{
	protected ref map<int, string> m_playerLoadouts = new map<int, string>();
	
	bool HasLoadout(GameEntity player)
	{
		return m_playerLoadouts.Contains(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
	}
	
	void SaveLoadout(GameEntity player)
	{
		auto playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
		
		if(playerId < 1) return;
		
		SCR_JsonSaveContext context();
	
		context.WriteGameEntity("LOADOUT_DATA", player);
		
		m_playerLoadouts.Set(playerId, context.ExportToString());
		
		// -----------------
		auto playerInventoryStorageManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		
		array<BaseInventoryStorageComponent> playerStorages();
		
		auto nStorages = playerInventoryStorageManager.GetStorages(playerStorages);
		
		Print(nStorages);
		
		foreach(auto playerStorage : playerStorages)
		{
			Print(playerStorage);
			
			array<BaseInventoryStorageComponent> subStorages();
			
			auto subStoragesLoaded = playerStorage.GetOwnedStorages(subStorages, 9999, false);
			
			Print(subStoragesLoaded);
			Print(subStorages);
		}
	}
	
	void RestoreLoadout(GameEntity player)
	{
		auto playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
		
		if(playerId < 1) return;
		
		SCR_JsonLoadContext context();
				
		context.ImportFromString(m_playerLoadouts.Get(playerId));

		context.ReadGameEntity("LOADOUT_DATA", player);
	}
}
