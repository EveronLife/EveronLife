[WorkbenchPluginAttribute("Clear Database", "Deletes the .db folder!", "", "", {"WorldEditor", "ResourceManager"}, "", 0xf1c0)]
class EL_ClearDatabasePlugin: WorkbenchPlugin
{
	const string DB_BASE_DIR = "$profile:/.db";

	//------------------------------------------------------------------------------------------------
	void DeleteFileCallback(string path, FileAttribute attributes)
	{
		FileIO.DeleteFile(path);
	}

	//------------------------------------------------------------------------------------------------
	override void Run()
	{
		System.FindFiles(DeleteFileCallback, DB_BASE_DIR, ".json");
		System.FindFiles(DeleteFileCallback, DB_BASE_DIR, ".bin");
		System.FindFiles(DeleteFileCallback, DB_BASE_DIR, string.Empty);
		Print("-- DATABASE CLEARED --", LogLevel.WARNING);
	}
};