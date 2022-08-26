class EL_Utils
{
	static string GetPlayerUID(IEntity player)
	{
		return GetPlayerUID(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
	}
	
	static string GetPlayerUID(int playerId)
	{
		string uid = GetGame().GetBackendApi().GetPlayerUID(playerId);
		if(!uid) uid = string.Format("LOCAL_UID_%1", playerId);
		return uid;
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
		if(!entity) return string.Empty;
		
		return SCR_BaseContainerTools.GetPrefabResourceName(entity.GetPrefabData().GetPrefab());
	}
	
	static IEntity FindEntityByRplId(RplId rplId)
	{
		IEntity entity = null;
		
		if (rplId.IsValid())
		{
			RplComponent entityRpl = RplComponent.Cast(Replication.FindItem(rplId));
			if (entityRpl) entity = IEntity.Cast(entityRpl.GetEntity());
		}
		
		return entity;
	}
	
	static void Teleport(IEntity entity, vector position, vector ypr = "-1 -1 -1", float scale = -1)
	{
		vector transform[4];
		
		if(ypr != "-1 -1 -1")
		{
			Math3D.AnglesToMatrix(ypr, transform);
			transform[3] = position;
		}
		else
		{
			entity.GetWorldTransform(transform);
			transform[3] = position;
			SCR_TerrainHelper.OrientToTerrain(transform);
		}
		
		if(scale != -1) SCR_Math3D.ScaleMatrix(transform, scale);

		TeleportTM(entity, transform);
	}
	
	static void TeleportTM(IEntity entity, vector transform[4])
	{
		BaseGameEntity baseGameEntity = BaseGameEntity.Cast(entity);
		if (baseGameEntity)
		{
			baseGameEntity.Teleport(transform);
		}
		else
		{
			entity.SetWorldTransform(transform);
		}
		
		Physics physics = entity.GetPhysics();
		if (physics)
		{
			physics.SetVelocity(vector.Zero);
			physics.SetAngularVelocity(vector.Zero);
		}
		
		if(!ChimeraCharacter.Cast(entity))
		{
			entity.Update();
		}
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
	
	static array<typename> SortTypenameHierarchy(array<typename> typenames)
	{
		map<typename, int> hierachyCount();
		
		foreach(typename possibleRootClass : typenames)
		{
			if(hierachyCount.Contains(possibleRootClass)) continue;
			
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
