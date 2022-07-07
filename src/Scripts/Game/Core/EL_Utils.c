class EL_Utils
{
	static IEntity SpawnEntityPrefab(ResourceName prefab, vector origin, vector orientation = "0 0 0")
	{
		EntitySpawnParams spawnParams();
		
		spawnParams.TransformMode = ETransformMode.WORLD;
		
		Math3D.AnglesToMatrix(orientation, spawnParams.Transform);
		spawnParams.Transform[3] = origin;
		
		return GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), spawnParams);
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
}

class EL_RefArrayCaster<Class TSourceType, Class TResultType>
{
	static array<ref TResultType> Convert(array<ref TSourceType> sourceArray)
	{
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
