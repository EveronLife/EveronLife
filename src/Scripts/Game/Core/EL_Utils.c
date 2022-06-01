class EL_Utils
{
	//------------------------------------------------------------------------------------------------
	static IEntity SpawnEntityPrefab(ResourceName prefab, vector origin, vector orientation = "0 0 0")
	{
		EntitySpawnParams spawnParams();
		
		spawnParams.TransformMode = ETransformMode.WORLD;
		
		Math3D.AnglesToMatrix(orientation, spawnParams.Transform);
		spawnParams.Transform[3] = origin;
		
		return GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), spawnParams);
	}
	
	//------------------------------------------------------------------------------------------------
	static void ChangeEntityMaterial(IEntity entity, ResourceName newMaterial)
	{
		if (!entity)
			return;
		
		VObject mesh = entity.GetVObject();
		if (!mesh)
			return;
		
		string remap;
		string materials[256];
		int numMats = mesh.GetMaterials(materials);
		if (numMats == 0)
			return;
		
		for (int i = 0; i < numMats; i++)
		{
			remap += string.Format("$remap '%1' '%2';", materials[i], newMaterial);
		}
		
		entity.SetObject(mesh, remap);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Workaround for missing FindComponent("MeshObject")
	static BaseContainer GetPrefabMeshComponent(ResourceName prefab) 
	{			
		BaseContainer meshComponent = null;
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
		
		if (!meshComponent)
			return null;
		
		return meshComponent;
	}
}
