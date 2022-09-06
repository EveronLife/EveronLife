# Setting up your development environment
This document is a detailed step by step guide to getting your development environment setup so you can start contributing to Everon Life.

### Prerequisites
- You must own [Arma Reforger](https://store.steampowered.com/app/1874880) and have it as well as the `Arma Reforger Tools` installed through Steam on your pc.
- Make sure you have launched the game at least once.

### Source code
In order to contribute, you need to download and work with the source code. You can obtain it by [cloning](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository) this repository to your local pc.
For this guide, the repository is cloned to `C:\EveronLife`, but you can choose a different folder.
Your chosen folder should now contain the same files you see on the main of the repository (e.g. `.gitignore` `/src` `/assets` etc).

### Setting up the Workbench
Most of the development workflows take place in the integrated developer environment for Arma Reforger called Workbench. You start it by launching the `Arma Reforger Tools` tools through Steam. To see all relevant logs during debugging you should add `-logLevel spam` into the start-up parameters in Steam and then selectively disable lower log levels you do not want to see in the console filter settings (top right corner above the log consoles).

#### Adding the project
When you launch the Workbench you are greeted with the project overview. To add EveronLife you want to click the `Add Existing` button on the bottom left and navigate to the folder you cloned into.
Now you go into `/src` and select the `EveronLife.gproj`. The full path could look like `C:\EveronLife\src\EveronLife.gproj`. The project list should now contain `EveronLife` and it should have a logo. 
Double click or select it and click `Open` on the bottom right.

#### Opening the debug world
You should now see the main window of the Workbench where you can quick launch all the different editors. Click the world editor button (left in the list) to open it.
Now while in the world editor, navigate to the top left and click `File > Load World`. In the popup window search for `DebugWorld`, select the `DebugWorld.ent` file, and open it with `Ok`.
The debug world is used to reduce loadtimes during development and testing of gameplay mechanics. The full Everon map (`MainWorld.ent`) takes significnatly longer to load and is only really needed for mapping the default multiplayer scenario.

#### Setting up the peer tool
With the debug world open, navigate to `Plugins > Settings > PeerTool`. Here you can configure the peer tool. It is used for local multiplayer testing by launching multiple game instances that automatically connect to your local server.
In the `Unsorted` category edit the `Executable` option and paste in 
```
../../Arma Reforger/ArmaReforgerSteam.exe
```
This will make the Workbench use the latest main game executable for testing. 
This path assumes that the game and the tools are installed in the same steam library drive. If you installed them in separate locations go and copy the `<your steam library>\steamapps\common\Arma Reforger\ArmaReforgerSteam.exe` from your main game to `<your other steam library>\steamapps\common\Arma Reforger Tools\Workbench\ArmaReforgerSteam.exe` and just put `ArmaReforgerSteam.exe` in the settings.

To add one or more instances for testing click the little `+` next to the `Peer Windows` settings. This adds appends a new instance of `PeerConfig`.
For each `PeerConfig` make sure that at least these values are present in the `Params`: 
```
-addonsDir "C:\EveronLife\src" -addons EveronLife
```

Optional: By default, the peer tool saves the logs to `C:\Users\<USER>\Documents\My Games\PeerPlugin<NUMBER>`. If you prefer to have them closer to your main instance logs you can fill this into `Profie`
```
ArmaReforger/_Peer
```
That way the logs are saved in `C:\Users\<USER>\Documents\My Games\ArmaReforger\_Peer<NUMBER>\profile` right next to your main logs that are located in `C:\Users\<USER>\Documents\My Games\ArmaReforger\profile`

#### Validating your setup
In the world editor in the top menu bar click the arrow next to the play button and choose `Server localhost + PeerTool` from the dropdown. Now enter the play mode in the Workbench. At least one additional game instance should be opening.
Once both are loaded, your players should have spawned and each be controllable by focussing either window.
