# Setting up your customized Everon Life server
This document is a detailed step-by-step guide to setting up a customized Everon Life server for your own community. 
It will go through the basic setup of how to make a mod based on the Everon Life framework, how to publish it and how to get it running on a dedicated server.

A video version of this is guide is available here *(more languages coming soon / additions welcome)*:
<kbd>[<img title="English" alt="English" src="https://cdn.staticaly.com/gh/hjnilsson/country-flags/master/svg/gb.svg" width="22">](https://www.youtube.com/watch?v=yNJNhTeZJN0)</kbd>

### Prerequisites
- You must own [Arma Reforger](https://store.steampowered.com/app/1874880) and have it as well as the `Arma Reforger Tools` and `Arma Reforger Server` installed on your pc.
- You will need a BI account to publish the workshop item. You can register one [here](https://accounts.bistudio.com) if needed.
- Make sure you have launched the game at least once.

### Subscribe to Everon Life 
The Everon Life workshop item will be the basis for your server. It contains all the base functionality and a small amount of roleplay-related assets to build your server with.
By building on top of the existing workshop item you can automatically receive new features, bug fixes, and compatibility updates for future game versions. 
Because of the way the framework is designed, you still have the freedom to customize all aspects of your server easily.
To subscribe to the item, launch the game and click `Workshop` in the main menu. On the top right enter `Everon Life` and download the workshop item published by `Everon Life Team`.
Now you can close the game.

### Custom server mod
To do custom mapping or to add any new assets (pictures, models, sounds, scripts) to your server you will need to have a mod specifically built for it. 
If you are coming from Arma 3 you might have previously done this via a so-called mission file that was supplied by the server. These do not exist anymore in Arma Reforger.
When people want to join your server any missing mods are automatically downloaded though, so there is no downside to you having a custom server mod.

#### Creating your custom server mod
Open the Workbench by launching `Arma Reforger Tools` via Steam. It shows up with a list of all your mod projects.

##### Project creation
First, you need to add `EveronLife` as a known dependency. To do this click the `Add Existing` button and navigate to `Documents\My Games\ArmaReforger\addons\59636E668EA37AD7` and select the `EveronLife.gproj`. Alternatively, if you downloaded the framework source directly, add that as an existing project.  
Now you want to create a new mod. To do this, click the bottom left `Create New` button. 
The Workbench will ask you for a project name. You can for example name it after your community. 
The name can be changed later with a bit of effort, but you can save yourself some time by getting it right the first time.
You can leave the location default value or save it somewhere else. It does not really matter.  
The **important** step here is to check the box next to `EveronLife` under `Dependencies`.
Now click `Ok`.

##### Preperations
The workbench opens and you are greeted with the main screen, showing you several quick launch buttons. First, you need to do some preparations. 
Select your mod folder in the top left `Resource Browser` and in the bottom half of it, you will see an `addon.gproj` file.
Right-click the `addon.gproj` and rename it to your mod name - e.g. `MyCustomServer.gproj`. (Should you restart the workbench you will need to add your project to the list again by selecting the renamed file)  
Right-click in the same bottom window and create a new folder called `Worlds` in it.

##### Creating the world file
Now you can use the quick launch button to open up the world editor. By default, it opens up a new empty world. Now you need to choose which map (world) you want to base your server on.
Navigate to `File > Load World` and search for the base world you want to use. For `Everon` you want to search for `Eden.ent` (`ArmaReforger > worlds > Eden > Eden.ent`). Select it and click `Ok`. 
After the map is loaded you navigate to `File > New World` and choose the option `Sub-scene (of current world)` and press `Ok`.
Once loaded you may or may not be in the middle of nowhere or in the water. Go into the left `Hierarchy` view unfold the parent world (e.g. `Eden`) and select any of the default layer objects. Now press F or right-click it and `Focus selection`. Now you should be able to see the map.
To make sure that you do not accidentally edit the parent world, you can right-click its layers (e.g. `default`) and choose `Lock`.
Before doing anything else you want to save the empty map via `File > Save World`. Navigate to your mod's `Worlds` folder (e.g. `MyCustomServer/Worlds`) and for the name, you can again put your community name (e.g. `MyCustomServer`). The file is then saved as `MyCustomServer.ent`
If it's not saved, save it via `File > Force Save All`. You should now see the files in your folder.

##### Adding the game mode
Now it is time to connect the map with the game mode. To do this, all you need to do is place two prefabs. Prefabs are a bundle of map objects and script components that are your primary building blocks when mapping for your server.
You need two prefabs for now. `GameMode_Roleplay.et` and `SpawnPoint_LIFE.et`. You can find both by searching for them in the `Resource Browser` at the bottom middle of your screen. You can locate them manually via `Prefabs > MP > Modes > Roleplay > GameMode_Roleplay.et` and `Prefabs > MP > Spawning > SpawnPoint_LIFE.et`.  
You simply drag and drop them from the `Resource Browser` into the world view above. They will appear in the `Hierarchy` view on the left afterward. Move the `SpawnPoint_LIFE` to where you want your players to spawn. The position of the `GameMode_Roleplay` entity does not matter. It is recommended to teleport it to 0 0 0 using the object transformation properties on the right when selected.  
To test if everything so far has been working, you can quickly press the play button in the top menu bar. You should spawn where you placed the spawn point and be able to control your player. You can close the world editor for now.

##### Setting up the mission config
For servers to be able to use the map you just created they need a mission. Back in the main workbench window, you want to add a folder next to your `Worlds` folder called `Missions`.
Go inside the folder and create a new mission config by doing `Rightclick > Create Resource > Config file`. You can again name this after your community. Hit Enter. Now it asks you which type of config you want to create.
Search for `SCR_MissionHeader` and select it.  
*Note: Because of a known issue you must provide your own (placeholder) image in the following step. Simply drag and drop a JPG or PNG from Windows into the `Resource Browser` to `MyCustomServer/Missions`. That will create the `.edds` file you need.*  
Now double click the newly created `.conf` file. Adjust the following settings:
- `World` -> Select the `.ent` world file you saved under `MyCustomServer/Worlds`
- `Name` -> Name that will be displayed in the mission section of the mod/server. You can just name it after your community.
- `Author` -> Name of the author. You personally or your community.
- `Game Mode` -> You might want to replace `Sandbox` with something like `Roleplay`
- `Icon` -> Choose the placeholder image mentioned before.
- `Loading Screen` -> Choose the placeholder image mentioned before.
- `Preview Image` -> Choose the placeholder image mentioned before.
- `Player Count` -> The amount of players the mission is designed for. You can just put `256` there.
- `Default Game Flags > Metabolism` -> Tick the box
Now save the file.

##### Optional: Finishing touches
1. If you want to, you can go back into the world editor, load up your map and create two new layers (layers can be created via right-click on your sub-scene) called `Gamemode` and `Spawnconfig` and move the `GameMode_Roleplay` and `SpawnPoint_LIFE` objects into their respective layer. 
This helps to keep things organized later. You can also lock those layers to not accidentally break the core functionality.

2. You can give your mod a project logo to be recognizable in the projects list. First, close the whole Workbench and launch it again.
If your mod is missing from the list, you need to add it again as an existing project by locating your `MODNAME.gproj` file.
Once it appears in the list again you right-click it and choose `Set thumbnail` to add a square `.png` as the logo for it.


### Publishing your mod
Your mod is now ready to be published. To publish it, first, open your mod project in the Workbench. Then make sure you are logged in with the correct user.
The user who publishes it owns it, and there seems to be **no way to transfer it later**. You might want to create a BI account just for your server team to share access.
You log in via `Workbench > Workshop Member Area > Login`

Now you navigate to `Workbench > Workshop Member Area > Publish project` and the publishing window will open.
- `Project Name` -> Could/Should be the same name as your project. If you are unsure about it, now would be a good time to decide on it.
- `Working Dir` -> A folder for temporary files. **Must not be the same as your source folder**. It is suggested you point it to something like `C:/Users/<USER>/Documents/My Games/ArmaReforger/publish/MyCustomServer/`
- `Preview Image` -> The image that is shown when browsing mods.
- `Screenshots` -> Screenshots that are shown on top of the mods detail page. Choose them with care or leave them empty for now. There might not be a way to delete them again.
- `Category` -> It is suggested you only tick `SCENARIOS_MP`
- `Tags` -> `Comma, Separated, List` of tags relevant to the mod. Could include your server name and things like `roleplay` and `life`
- `Version` -> It is recommended to change this from `1.0.0` to something like `0.0.1` while you are still early in your server's development. The versions increase automatically later. You can not choose a different version pattern.
- `Visibility` -> You can have it on public or even only as unlisted. When joining your server it will be downloaded regardless.
- `Summary` -> Short summary of the mod shown in the mod browsing section.
- `Description` -> Full description shown on the mod detail page. This would be one possible way **to correctly give credits** for using the Everon Life framework, by including something like `Powered by` or `Built with` `Everon Life Framework` 
Now click `Bundle` and then `Publish`

If you get the error message: `Asset bundle manifest is invalid` when publishing.
It could be because some characters are not supported in the workshop.
Make sure that the Summary and Description do not contain any illegal characters!


### Setting up your server
Before setting up your server you need two pieces of information: Your mod GUID and the path of your mission config.  
You get the `Mod GUID` by double-clicking your `MyCustomServer.gproj` file while inside the workbench. It opens up a window called `Options` which will display the GUID as `GUID` property under `Unsorted` at the very top.
The `Mission config path` can be optiained by navigating to your `MyCustomServer.conf` file inside `MyCustomServer/Missions` rightclicking the file and choosing `Utils > Copy Resource Name(s)`

Go into the installation directory of your dedicated server e.g. `C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger Server` and create a file called `launch.json` in the same folder the `ArmaReforgerServer.exe` is located in and put the following content into it:
```json
{
	"dedicatedServerId": "",
	"region": "EU",
	"gameHostBindAddress": "",
	"gameHostBindPort": 2001,
	"gameHostRegisterBindAddress": "",
	"gameHostRegisterPort": 2001,
	"adminPassword": "roleplayftw",
	"game": {
		"name": "Roleplay - MyCustomServer",
		"password": "",
		"scenarioId": "{5C552F6B07B10383}Missions/MyCustomServer.conf", <----- Mission config path mentioned in the previous step
		"playerCountLimit": 32,
		"autoJoinable": false,
		"visible": true,
		"gameProperties": {
			"serverMaxViewDistance": 2500,
			"serverMinGrassDistance": 50,
			"networkViewDistance": 1000,
			"disableThirdPerson": true,
			"fastValidation": true,
			"battlEye": true
		},
		"mods": [
			{
				"modId": "597563C79DA08617", <------------ Mod GUID you located before this step
				"name": "MyCustomServer", <--------------- Name of the mod you entered in the publisher window
				"version": "0.0.1" <---------------------- Your mod version here (likely to be 0.0.1 at first if you followed this guide.
			}
		]
	},
	"a2sQueryEnabled": true,
	"steamQueryPort": 17777
}
```
Make sure to remove the helper arrows from the file before saving.
Now create a file called `launch.bat` and put the following content into it:
```batch
ArmaReforgerServer.exe -config ".\launch.json" -profile ArmaReforgerServer
```
Doubleclick the `launch.bat` and wait for your server to start. It will automatically download your custom mod and save everything in `C:\Users\<USER>\Documents\My Games\ArmaReforgerServer`.
You can find more parameters and information about the dedicated server setup [here](https://community.bistudio.com/wiki/Arma_Reforger:Server_Hosting).

### Joining your server
The last step is quite simple. Launch your game, choose `Multiplayer` and use `Direct join` or search for the server. 
When you attempt to join it will prompt you to download the mods it uses.
**Note**: If the mod download appears to be stuck wait a few seconds and then press ESC to exit the overlay. Open `Workshop` and go the the `Downloaded` tab. There you should see your mod. Click onto it and download any updates available and make sure it is `Enabled` (toggle in the top left corner). Now attempt to join the server again and it should work.

### Something is not working?
Go through the steps again, maybe you overlooked something. Otherwise, compare your setup with the example files repository that can be found [here](https://github.com/EveronLife/MyCustomServer).
If that does not help, reach out to us via discussions or [Discord](https://discord.gg/everonlife)
