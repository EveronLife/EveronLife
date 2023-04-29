class EL_BitFlags
{
	//------------------------------------------------------------------------------------------------
	static void SetFlags(inout int value, int flags)
	{
		value |= flags;
	}

	//------------------------------------------------------------------------------------------------
	static void ClearFlags(inout int value, int flags)
	{
		value &= ~flags;
	}

	//------------------------------------------------------------------------------------------------
	static void ToggleFlags(inout int value, int flags)
	{
		value ^= flags;
	}

	//------------------------------------------------------------------------------------------------
	static bool CheckFlags(int value, int flags, bool exactFlags = false)
	{
		if (exactFlags) return (value & flags) == flags;
		return (value & flags);
	}
};
