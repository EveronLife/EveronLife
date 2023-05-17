enum EL_EPersistenceFlags
{
	// Current status
	INITIALIZED			= 1,
	BAKED				= 2,
	ROOT				= 4,
	PERSISTENT_RECORD	= 8,
	PAUSE_TRACKING		= 16,

	// Permanent event memory
	WAS_MOVED			= 32,
	WAS_EQUIPPED		= 64,
	WAS_SELECTED		= 128,
};
