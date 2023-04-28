class EL_ReflectionUtils<Class T>
{
	protected static ref map<string, int> s_mIdxCache;

	//------------------------------------------------------------------------------------------------
	static bool Get(notnull Class instance, string variableName, out T value)
	{
		typename type = instance.Type();
		int vIdx = GetVIdx(type, variableName);
		return type.GetVariableValue(instance, vIdx, value);
	}

	//TODO: Add Set() via Json serializer
	
	//------------------------------------------------------------------------------------------------
	static int GetVIdx(typename type, string fieldName)
	{
		int idx;

		if (!s_mIdxCache)
			s_mIdxCache = new map<string, int>();

		string key = type.ToString() + "::" + fieldName;
		if (!s_mIdxCache.Find(key, idx))
		{
			idx = -1;
			for (int vIdx = 0; vIdx < type.GetVariableCount(); vIdx++)
			{
				if (type.GetVariableName(vIdx) == fieldName)
				{
					idx = vIdx;
					s_mIdxCache.Set(key, idx);
					break;
				}
			}
		}

		return idx;
	}
};
