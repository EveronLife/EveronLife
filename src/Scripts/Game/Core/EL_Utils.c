class EL_Utils
{
	static string GetPlayerUID(IEntity player)
	{
		return GetPlayerUID(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
	}
	
	static string GetPlayerUID(int playerId)
	{
		#ifdef WORKBENCH
		return "MAIN_WORKBENCH_GUID";
		#endif
		
		string peerToolUid;
		if(System.GetCLIParam("peer-tool-uid", peerToolUid))
		{
			return peerToolUid;
		}
		
		return GetGame().GetBackendApi().GetPlayerUID(playerId);
	}
	
	static IEntity SpawnEntityPrefab(ResourceName prefab, vector origin, vector orientation = "0 0 0")
	{
		EntitySpawnParams spawnParams();
		
		spawnParams.TransformMode = ETransformMode.WORLD;
		
		Math3D.AnglesToMatrix(orientation, spawnParams.Transform);
		spawnParams.Transform[3] = origin;
		
		return GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), spawnParams);
	}
	
	static ResourceName GetPrefabName(IEntity entity)
	{
		return SCR_BaseContainerTools.GetPrefabResourceName(entity.GetPrefabData().GetPrefab());
	}
	
	static string IntToHex(int value)
	{
		array<string> resultChars = {"0", "0", "0", "0", "0", "0", "0", "0"};
		
		int resultIdx = 7;
		
        while (value) 
		{
            int remainder = value % 16;
 
            if (remainder < 10) 
			{
                resultChars.Set(resultIdx--, remainder.ToString());
            }
            else
			{
                resultChars.Set(resultIdx--, (remainder + 55).AsciiToString());
            }
 			
            value /= 16;
        }
		
		string result;
		
		foreach(string char : resultChars)
		{
			result += char;
		}
		
		return result;
	}
	
	static DateTimeUtcAsInt GetCurrentUtcAsInt()
	{
		int year, month, day, hour, minute, second;
		System.GetYearMonthDayUTC(year, month, day);
		System.GetHourMinuteSecondUTC(hour, minute, second);
		return ((year - 2000) << 26) | (month << 22) | (day << 17) | (hour << 12) | (minute << 6) | second;
	}
	
	static array<typename> SortTypenameHierarchy(array<typename> typenames)
	{
		map<typename, int> hierachyCount();
		
		foreach(typename possibleRootClass : typenames)
		{
			hierachyCount.Set(possibleRootClass, 0);
			
			foreach(typename compareRootClass : typenames)
			{
				if(possibleRootClass.IsInherited(compareRootClass))
				{
					hierachyCount.Set(possibleRootClass, hierachyCount.Get(possibleRootClass) + 1);
				}
			}
		}
		
		array<string> sortedHierachyTuples();
		sortedHierachyTuples.Resize(hierachyCount.Count());
		
		int hierachyIdx = 0;
		foreach(typename type, int count : hierachyCount)
		{
			sortedHierachyTuples.Set(hierachyIdx++, string.Format("%1:%2", count.ToString(3), type.ToString()));
		}
		
		sortedHierachyTuples.Sort(true);
		
		array<typename> sortedHierachy();
		sortedHierachy.Resize(hierachyCount.Count());
		
		foreach(int idx, string tuple : sortedHierachyTuples)
		{
			int typenameStart = tuple.IndexOf(":") + 1;
			string typenameString = tuple.Substring(typenameStart, tuple.Length() - typenameStart);
			sortedHierachy.Set(idx, typenameString.ToType());
		}
		
		return sortedHierachy;
	}
}

class EL_RefArrayCaster<Class TSourceType, Class TResultType>
{
	static array<ref TResultType> Convert(array<ref TSourceType> sourceArray)
	{
		if(!sourceArray) return null;
		
		array<ref TResultType> castedResult();
		castedResult.Resize(sourceArray.Count());
		
		foreach(int idx, TSourceType element : sourceArray)
		{
			TResultType castedElement = TResultType.Cast(element);
			
			if(castedElement) castedResult.Set(idx, castedElement);
		}
		
		return castedResult;
	}
}
