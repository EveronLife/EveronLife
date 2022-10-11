# Whitelist System
### At server startup
When the server starts, the whitelist system will read the file `whitelist.txt` which is in the profile (`$profile`).

If the file is not present, the whitelist system is not active.

The `whitelist.txt` file has a simple form, one UID (BI) per line:
```
xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
```

The system will store all the uuids present in the file in the database and delete from the database if they are absent from the file.

The database is used for future use (API, management via interface), for the moment only the file allows to add or remove a user in the whitelist.

A summary is printed in the logs in `Debug` format, in the following form:
```
UUID Read : (File : X : Database : X) | UUID add to list: X | UUID remove from list : x
```

### When a player logs in
For each person who connects to the server, the whitelist system will read the database and check if the uuid of the player is present.

If the player is not present, he will be automatically returned to the main menu of the game, and a `Warning` log will be displayed in the console.
