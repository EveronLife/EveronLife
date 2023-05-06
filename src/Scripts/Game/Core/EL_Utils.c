class EL_Utils
{
	//------------------------------------------------------------------------------------------------
	//! Gets the Bohemia UID
	//! \param playerId Index of the player inside player manager
	//! \return the uid as string
	static string GetPlayerUID(int playerId)
	{
		if (!Replication.IsServer())
		{
			Debug.Error("GetPlayerUID can only be used on the server.");
			return string.Empty;
		}

		string uid = GetGame().GetBackendApi().GetPlayerUID(playerId);
		if (!uid)
		{
			if (RplSession.Mode() == RplMode.Dedicated)
			{
				Debug.Error("Dedicated server is not correctly configuted to connect to the BI backend.\nSee https://community.bistudio.com/wiki/Arma_Reforger:Server_Hosting#gameHostRegisterBindAddress");
			}
			else
			{
				uid = string.Format("LOCAL_UID_%1", playerId);
			}
		}

		return uid;
	}

	//------------------------------------------------------------------------------------------------
	//! Gets the Bohemia UID
	//! \param player Instance of the player
	//! \return the uid as string
	static string GetPlayerUID(IEntity player)
	{
		return GetPlayerUID(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
	}

	//------------------------------------------------------------------------------------------------
	//! Spawns a prefab
	//! \param prefab ResournceName of the prefab to be spawned
	//! \param origin Position(origin) where to spawn the entity
	//! \param orientation Angles(yaw, pitch, rolle in degrees) to apply to the entity
	//! \return the spawned entity or null on failure
	static IEntity SpawnEntityPrefab(ResourceName prefab, vector origin, vector orientation = "0 0 0", bool global = true)
	{
		EntitySpawnParams spawnParams();

		spawnParams.TransformMode = ETransformMode.WORLD;

		Math3D.AnglesToMatrix(orientation, spawnParams.Transform);
		spawnParams.Transform[3] = origin;

		if (!global) return GetGame().SpawnEntityPrefabLocal(Resource.Load(prefab), GetGame().GetWorld(), spawnParams);

		return GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), spawnParams);
	}

	//------------------------------------------------------------------------------------------------
	//! Gets the prefab the entity uses
	//! \param entity Instance of which to get the prefab name
	//! \return the resource name of the prefab or empty string if no prefab was used or entity is invalid
	static ResourceName GetPrefabName(IEntity entity)
	{
		if (!entity) return string.Empty;
		return SCR_BaseContainerTools.GetPrefabResourceName(entity.GetPrefabData().GetPrefab());
	}

	//------------------------------------------------------------------------------------------------
	//! Gets the prefab the entity uses or the unique map name
	//! \param entity Instance of which to get the prefab/map name
	//! \return the resource name of the prefab or name on the map (might be empt)
	static string GetPrefabOrMapName(IEntity entity)
	{
		if (!entity) return string.Empty;
		string name = entity.GetPrefabData().GetPrefabName();
		if (!name) name = entity.GetName();
		return name;
	}

	//------------------------------------------------------------------------------------------------
	//! Teleport an entity (and align it to terrain if no angles are specified)
	//! \param entity Entity instance to be teleported
	//! \param position Position where to teleport to
	//! \param angles (yaw, pitch, rolle in degrees) to apply after teleportation
	static void Teleport(notnull IEntity entity, vector position, float yaw = float.INFINITY)
	{
		vector transform[4];

		if (yaw != float.INFINITY)
		{
			Math3D.AnglesToMatrix(Vector(yaw, 0 ,0), transform);
		}
		else
		{
			entity.GetWorldTransform(transform);
		}

		transform[3] = position;
		SCR_TerrainHelper.OrientToTerrain(transform);

		ForceTransform(entity, transform);
	}

	//------------------------------------------------------------------------------------------------
	//! Force entity into a new transformation matrix
	//! \param entity
	//! \param origin
	//! \param angles (yaw, pitch, roll in degrees)
	//! \param scale
	static void ForceTransform(notnull IEntity entity, vector origin = EL_Const.VEC_INFINITY, vector angles = EL_Const.VEC_INFINITY, float scale = float.INFINITY)
	{
		vector transform[4];
		entity.GetWorldTransform(transform);

		if (origin != EL_Const.VEC_INFINITY)
		{
			transform[3] = origin;
		}

		if (angles != EL_Const.VEC_INFINITY)
		{
			Math3D.AnglesToMatrix(angles, transform);
		}

		if (scale != float.INFINITY)
		{
			SCR_Math3D.ScaleMatrix(transform, scale);
		}

		ForceTransform(entity, transform);
	}

	//------------------------------------------------------------------------------------------------
	//! Set the transformation matrix and update the entity
	static void ForceTransform(IEntity entity, vector transform[4])
	{
		BaseGameEntity baseGameEntity = BaseGameEntity.Cast(entity);
		if (baseGameEntity && !BaseVehicle.Cast(baseGameEntity))
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

		if (!ChimeraCharacter.Cast(entity))
		{
			entity.Update();
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Convert integer to hexadeciamal string
	//! \param value Input integer
	//! \param upperCase Decide if output should be upper or lower case
	//! \param fixedLength Add leading zeros for a minimum length output
	//! \return result hexadecimal string
	static string IntToHex(int value, bool upperCase = false, int fixedLength = -1)
	{
		array<string> resultChars = {"0", "0", "0", "0", "0", "0", "0", "0"};

		int asciiOffset = 87;
		if (upperCase) asciiOffset = 55;

		int padUntil = 7;
		if (fixedLength != -1) padUntil = 8 - Math.Min(fixedLength, 8);

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
				resultChars.Set(resultIdx--, (remainder + asciiOffset).AsciiToString());
			}

			value /= 16;
		}

		string result;
		bool nonZero;

		foreach (int nChar, string char : resultChars)
		{
			if (char == "0" && nChar < padUntil && !nonZero) continue;
			nonZero = true;
			result += char;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	//! Sort an array of typenames by their inheritance on each other in descending order
	//! \param typenames Input typenames
	//! \return sorted distinct typenames
	static array<typename> SortTypenameHierarchy(array<typename> typenames)
	{
		map<typename, int> hierachyCount();

		foreach (typename possibleRootClass : typenames)
		{
			hierachyCount.Set(possibleRootClass, 0);
		}

		foreach (typename type, int count : hierachyCount)
		{
			foreach (typename compareType, auto _ : hierachyCount)
			{
				if (type.IsInherited(compareType)) count++;
			}

			hierachyCount.Set(type, count);
		}

		array<string> sortedHierachyTuples();
		sortedHierachyTuples.Reserve(hierachyCount.Count());

		foreach (typename type, int count : hierachyCount)
		{
			sortedHierachyTuples.Insert(string.Format("%1:%2", count.ToString(3), type.ToString()));
		}

		sortedHierachyTuples.Sort(true);

		array<typename> sortedHierachy();
		sortedHierachy.Reserve(hierachyCount.Count());

		foreach (string tuple : sortedHierachyTuples)
		{
			int typenameStart = tuple.IndexOf(":") + 1;
			string typenameString = tuple.Substring(typenameStart, tuple.Length() - typenameStart);
			sortedHierachy.Insert(typenameString.ToType());
		}

		return sortedHierachy;
	}

	//------------------------------------------------------------------------------------------------
	static int MaxInt(int a, int b)
	{
		if (a > b) return a;
		return b;
	}

	//------------------------------------------------------------------------------------------------
	static int MinInt(int a, int b)
	{
		if (a < b) return a;
		return b;
	}

	//------------------------------------------------------------------------------------------------
	static bool IsAnyInherited(Class instance, notnull array<typename> from)
	{
		if (instance)
		{
			typename type = instance.Type();
			foreach (typename candiate : from)
			{
				if (type.IsInherited(candiate)) return true;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns a new string with the last "amount" of characters removed
	static string TrimEnd(string data, int amount)
	{
		return data.Substring(0, data.Length() - amount);
	}
};

class EL_RefArrayCaster<Class TSourceType, Class TResultType>
{
	//------------------------------------------------------------------------------------------------
	//! Cast an array by converting all individual items. Allocates a new array of the input size.
	//! \param sourceArray Input array
	//! \return casted result array
	static array<ref TResultType> Convert(array<ref TSourceType> sourceArray)
	{
		if (!sourceArray) return null;

		array<ref TResultType> castedResult();
		castedResult.Reserve(sourceArray.Count());

		foreach (TSourceType element : sourceArray)
		{
			TResultType castedElement = TResultType.Cast(element);

			if (castedElement) castedResult.Insert(castedElement);
		}

		return castedResult;
	}
};
