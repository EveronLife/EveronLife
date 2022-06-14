class AdminClass: GenericEntityClass
{
}

class Admin: GenericEntity
{
	override protected void EOnInit(IEntity owner)
    {
		PlayerManager playerManager = GetGame().GetPlayerManager();
		array<int> players = {};
		playerManager.GetAllPlayers(players);
		Print("ALLES SPIELER::::::::::::::::::::::::::: " + players);
        super.EOnInit(owner);
        auto ctx = GetGame().GetRestApi().GetContext("https://webhook.site/a5da2224-4a40-471c-a1dd-dca8f5643ac7/");
        TestCallback callback();
        ctx.GET(callback,"RequestPath?Argument=Something");

    }
}
