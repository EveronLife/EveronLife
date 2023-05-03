class EL_ReflectionUtils
{
	static typename GetAttributeParent()
	{
		string callStack;
		Debug.DumpStack(callStack);
		int startRead = callStack.LastIndexOf("#CreateAttributes") + 17;
		int stopRead = callStack.IndexOfFrom(startRead, "(");
		return callStack.Substring(startRead, stopRead - startRead).ToType();
	}
};

class EL_ReflectionUtilsT<Class T>
{
	//------------------------------------------------------------------------------------------------
	static bool Get(notnull Class instance, string variableName, out T value)
	{
		EL_ReflectionVariableInfo info = EL_ReflectionVariableInfo.Get(instance, variableName);
		if (!info)
			return false;
		return info.m_tHolderType.GetVariableValue(instance, info.m_iVariableindex, value);
	}

	//------------------------------------------------------------------------------------------------
	//TODO: Add Set() via Json serializer
};

enum EL_ReflectionVariableType
{
	NONE,
	ARRAY,
	SET,
	MAP
};

class EL_ReflectionVariableInfo
{
	protected static ref map<string, ref EL_ReflectionVariableInfo> s_mTypeCache;

	int m_iVariableindex;
	typename m_tVaribleType;
	typename m_tHolderType;
	typename m_tCollectionKeyType;
	typename m_tCollectionValueType;
	EL_ReflectionVariableType m_eCollectionType;

	//------------------------------------------------------------------------------------------------
	static EL_ReflectionVariableInfo Get(notnull Class instance, string fieldName)
	{
		if (!s_mTypeCache)
			s_mTypeCache = new map<string, ref EL_ReflectionVariableInfo>();

		typename type = instance.Type();
		string typeCacheKey = string.Format("%1::%2", type.ToString(), fieldName);
		EL_ReflectionVariableInfo info = s_mTypeCache.Get(typeCacheKey);
		if (!info)
		{
			for (int vIdx = 0; vIdx < type.GetVariableCount(); vIdx++)
			{
				if (type.GetVariableName(vIdx) == fieldName)
				{
					info = new EL_ReflectionVariableInfo(vIdx, type.GetVariableType(vIdx), type);
					s_mTypeCache.Set(typeCacheKey, info);
					break;
				}
			}
		}

		return info;
	}

	//------------------------------------------------------------------------------------------------
	protected void EL_ReflectionVariableInfo(int variableIndex, typename variableType, typename holderType)
	{
		m_iVariableindex = variableIndex;
		m_tVaribleType = variableType;
		m_tHolderType = holderType;

		if (m_tVaribleType.IsInherited(array) || m_tVaribleType.IsInherited(set))
		{
			if (m_tVaribleType.IsInherited(array))
			{
				m_eCollectionType = EL_ReflectionVariableType.ARRAY;
			}
			else
			{
				m_eCollectionType = EL_ReflectionVariableType.SET;
			}

			string typeString = m_tVaribleType.ToString();
			typeString.Replace("@", "");

			int templateStart = typeString.IndexOf("<") + 1;
			string collectionValueTypeString = typeString.Substring(templateStart, typeString.Length() - templateStart - 1);
			m_tCollectionValueType = collectionValueTypeString.ToType();
		}
		else if (m_tVaribleType.IsInherited(map))
		{
			m_eCollectionType = EL_ReflectionVariableType.MAP;

			string typeString = m_tVaribleType.ToString();
			typeString.Replace("@", "");

			int keyTypeStart = typeString.IndexOf("<") + 1;
			int valueTypeStart = typeString.IndexOfFrom(keyTypeStart, ",") + 1;

			string collectionKeyTypeString = typeString.Substring(keyTypeStart, valueTypeStart - keyTypeStart - 1);
			m_tCollectionKeyType = collectionKeyTypeString.ToType();

			string collectionValueTypeString = typeString.Substring(valueTypeStart, typeString.Length() - valueTypeStart - 1);
			m_tCollectionValueType = collectionValueTypeString.ToType();
		}
	}
};
