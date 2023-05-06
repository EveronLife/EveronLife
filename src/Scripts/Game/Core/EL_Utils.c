class EL_Utils
{
	static ResourceName PLACEHOLDER_ICON = "{AC7E384FF9D8016A}Common/Textures/placeholder_BCR.edds";

	//------------------------------------------------------------------------------------------------
	static void ChangeColorRecursive(IEntity parent, Color color)
	{
		IEntity child = parent.GetChildren();
		while (child)
		{
			if (child.GetChildren())
				ChangeColorRecursive(child, color);

			SetColor(child, color);
			child = child.GetSibling();
		}
	}

	//------------------------------------------------------------------------------------------------
	static void SetSlotsColor(notnull IEntity entity, int color)
	{
		SlotManagerComponent slotManager = EL_ComponentFinder<SlotManagerComponent>.Find(entity);
		if (!slotManager)
			return;
		array<EntitySlotInfo> slots = new array<EntitySlotInfo>;
		slotManager.GetSlotInfos(slots);
		foreach (EntitySlotInfo slotInfo : slots)
		{
			IEntity slotEnt = slotInfo.GetAttachedEntity();
			if (!slotEnt)
				continue;

			EL_VehicleAppearanceComponent slotAppearance = EL_ComponentFinder<EL_VehicleAppearanceComponent>.Find(slotEnt);
			if (slotAppearance)
				slotAppearance.SetVehicleColor(color);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Creates and sets new material with given color to entity
	//! \param entity Entity to set new color material
	//! \param color Color for the new material
	static void SetColor(notnull IEntity entity, Color color)
	{
		
		VObject mesh = entity.GetVObject();
			
		if (!mesh || !color)
			return;

		string materialName = "vehicle_material_" + entity;

		float materialColorRGBA[] = { color.R(), color.G(), color.B(), color.A() };

		
		Material dynamicColorMaterial = Material.Create(materialName, "MatPBRMulti");
		
		dynamicColorMaterial.SetParam("Color", materialColorRGBA);
		
		string matName;
		dynamicColorMaterial.GetName(matName);
		
		string remap;
		string materials[256];
		
		int numMats = mesh.GetMaterials(materials);
		if (numMats == 0)
			return;

		for (int i = 0; i < numMats; i++)
		{
			string originalMatName = materials[i];
			if (originalMatName.EndsWith("Body") || originalMatName.EndsWith("Base") || originalMatName.EndsWith("Body_Roof"))
			{
				
				remap += string.Format("$remap '%1' '%2';", materials[i], matName);
			}
		}
		entity.SetObject(mesh, remap);
	}

	//------------------------------------------------------------------------------------------------
	//! Gets prefab VObject
	//! \param prefab Prefab path
	//! \return the VObject of the prefab
	static VObject GetPrefabVObject(ResourceName prefab)
	{
		BaseContainer meshComponent;
		IEntitySource prefabSource = Resource.Load(prefab).GetResource().ToEntitySource();
		int count = prefabSource.GetComponentCount();

		for(int i = 0; i < count; i++)
		{
			IEntityComponentSource comp = prefabSource.GetComponent(i);

			if(comp.GetClassName() == "MeshObject")
			{
				meshComponent = comp;
				break;
			}
		}
		ResourceName prefabObject;
		meshComponent.Get("Object", prefabObject);
		return Resource.Load(prefabObject).GetResource().ToVObject();
	}


	//------------------------------------------------------------------------------------------------
	//! Gets the Bohemia UID
	//! \param playerId Index of the player inside player manager
	//! \return the uid as string
	static string GetPlayerUID(int playerId)
	{
		string uid = GetGame().GetBackendApi().GetPlayerUID(playerId);
		if (!uid)
		{
			if (RplSession.Mode() == RplMode.Dedicated)
				Print("Error getting uid for playerId: " + playerId, LogLevel.ERROR);
			uid = string.Format("LOCAL_UID_%1", playerId);
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
	static IEntity SpawnEntityPrefab(ResourceName prefab, vector origin, vector orientation = "0 0 0", IEntity parent = null, bool global = true)
	{
		EntitySpawnParams spawnParams();

		spawnParams.TransformMode = ETransformMode.WORLD;

		Math3D.AnglesToMatrix(orientation, spawnParams.Transform);
		spawnParams.Transform[3] = origin;
		spawnParams.Parent = parent;

		IEntity newEnt;
		if (!global)
			newEnt = GetGame().SpawnEntityPrefabLocal(Resource.Load(prefab), GetGame().GetWorld(), spawnParams);
		else
			newEnt = GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), spawnParams);

		if (parent)
			parent.AddChild(newEnt, -1);
		return newEnt;
	}

	//------------------------------------------------------------------------------------------------
	//! Gets the prefab the entitty uses
	//! \param entity Instance of which to get tthe prefab name
	//! \return the resource name of the prefab or empty string if no prefab was used or entity is invalid
	static ResourceName GetPrefabName(IEntity entity)
	{
		if (!entity) return string.Empty;
		return SCR_BaseContainerTools.GetPrefabResourceName(entity.GetPrefabData().GetPrefab());
	}

	//------------------------------------------------------------------------------------------------
	//! Teleport an entity
	//! \param entity Entity instance to be teleported
	//! \param position Position where to teleport to
	//! \param ypr Angles(yaw, pitch, rolle in degrees) to apply after teleportation
	//! \param scale Transformation scale to apply after teleportation
	static void Teleport(IEntity entity, vector position, vector ypr = "-1 -1 -1", float scale = -1)
	{
		vector transform[4];

		if (ypr != "-1 -1 -1")
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

		if (scale != -1) SCR_Math3D.ScaleMatrix(transform, scale);

		TeleportTM(entity, transform);
	}

	//------------------------------------------------------------------------------------------------
	//! Teleport an entity
	//! \param entity Entity instance to be teleported
	//! \param transform Target transformation matrix
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
			if (hierachyCount.Contains(possibleRootClass)) continue;

			hierachyCount.Set(possibleRootClass, 0);

			foreach (typename compareRootClass : typenames)
			{
				if (possibleRootClass.IsInherited(compareRootClass))
				{
					hierachyCount.Set(possibleRootClass, hierachyCount.Get(possibleRootClass) + 1);
				}
			}
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
		if(a > b) return a;
		return b;
	}

	//------------------------------------------------------------------------------------------------
	static int MinInt(int a, int b)
	{
		if(a < b) return a;
		return b;
	}

	//------------------------------------------------------------------------------------------------
	static bool IsAnyInherited(Class instance, notnull array<typename> from)
	{
		typename type = instance.Type();
		foreach (typename candiate : from)
		{
			if (type.IsInherited(candiate)) return true;
		}
		return false;
	}
}

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
}
