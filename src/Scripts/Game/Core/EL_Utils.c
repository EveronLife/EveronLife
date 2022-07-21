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
	
	static IEntity SpawnAsChild(ResourceName prefab, notnull IEntity parent, notnull PivotPoint pivot)
	{
		
		Resource resource = Resource.Load(prefab);
		
		if (!resource.IsValid()) return null;
		
		IEntity child = GetGame().SpawnEntityPrefab(resource, parent.GetWorld());
		parent.AddChild(child, parent.GetBoneIndex(pivot.GetName()));
		
		vector tm[4];
		pivot.GetLocalTransform(tm);
		child.SetLocalTransform(tm);
		
		return child;
	}
}

[BaseContainerProps()]
class PivotPoint
{
	protected const int POSITION = 3;
	
	[Attribute(desc: "Name of the pivot point, if not specified, default is origin")]
	protected string m_PivotName;
	
	[Attribute()]
	protected vector m_Offset;
	
	[Attribute()]
	protected vector m_Rotation;
	
	string GetName()
	{
		return m_PivotName;
	}
	
	vector GetOffset()
	{
		return m_Offset;
	}
	
	void GetLocalTransform(out vector transform[4])
	{
		Math3D.AnglesToMatrix(m_Rotation, transform);
		transform[POSITION] = m_Offset;
	}
}
