[EntityEditorProps(category: "EveronLife/Core/GameMode", description: "Core functionality")]
class EL_GameModeRoleplayClass: SCR_BaseGameModeClass
{
}

class EL_GameModeRoleplay: SCR_BaseGameMode
{
	void ~EL_GameModeRoleplay()
	{
		EL_PlayerAccountManager.Reset();
	}
}
