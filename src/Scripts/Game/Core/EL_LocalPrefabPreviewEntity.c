[EntityEditorProps(category: "GameScripted/Preview", color: "0 0 0 0", dynamicBox: true)]
class EL_LocalPrefabPreviewEntityClass: SCR_PrefabPreviewEntityClass
{
};
/*!
Preview entity created from entity prefab.
*/
class EL_LocalPrefabPreviewEntity: SCR_PrefabPreviewEntity
{
	//------------------------------------------------------------------------------------------------
	static SCR_BasePreviewEntity SpawnLocalPreviewFromPrefab(Resource prefabResource, ResourceName previewPrefab, vector origin, SCR_BasePreviewEntity rootEntity = null)
	{
		return SpawnLocalPreview(GetPreviewEntriesFromPrefab(prefabResource), previewPrefab, origin, rootEntity);
	}

	//------------------------------------------------------------------------------------------------
	static SCR_BasePreviewEntity SpawnLocalPreview(notnull array<ref SCR_BasePreviewEntry> entries, ResourceName previewResource, vector origin, SCR_BasePreviewEntity rootEntity = null)
	{
		if (entries.IsEmpty())
		{
			Print("No entries defined!", LogLevel.WARNING);
			return null;
		}

		if (previewResource.IsEmpty())
		{
			Print("No previewResource defined!", LogLevel.WARNING);
			return null;
		}

		World world = GetGame().GetWorld();

		EntitySpawnParams spawnParamsLocal = new EntitySpawnParams();
		spawnParamsLocal.Transform[3] = origin;
		if (!rootEntity)
			rootEntity = SCR_BasePreviewEntity.Cast(GetGame().SpawnEntityPrefabLocal(Resource.Load(previewResource), world, spawnParamsLocal));

		array<SCR_BasePreviewEntity> children = {};
		SCR_BasePreviewEntity entity, parent;
		foreach (SCR_BasePreviewEntry entry: entries)
		{
			spawnParamsLocal = new EntitySpawnParams();
			entry.LoadTransform(spawnParamsLocal.Transform);

			if (entry.m_iParentID == -1)
				parent = rootEntity;
			else
				parent = children[entry.m_iParentID];

			//--- Create slot entity
			entity = SCR_BasePreviewEntity.Cast(GetGame().SpawnEntityPrefabLocal(Resource.Load(previewResource), world, spawnParamsLocal));

			//--- Set mesh from a file
			if (entity && entry.m_Mesh)
			{
				entity.SetObject(Resource.Load(entry.m_Mesh).GetResource().ToVObject(), "");
			}

			children.Insert(entity);

			//--- Add to parent (spawn params won't do that on their own)
			int pivot = -1;
			if (!entry.m_iPivotID.IsEmpty())
				pivot = parent.GetBoneIndex(entry.m_iPivotID);
			parent.AddChild(entity, pivot, EAddChildFlags.AUTO_TRANSFORM);
			
			//--- Cache the child in parent's array
			if (!parent.m_aChildren)
				parent.m_aChildren = {};
			parent.m_aChildren.Insert(entity);
						
			entity.m_Entity = entry.m_Entity;
			
			entity.EOnPreviewInit(entry, rootEntity);
		}
				
		return rootEntity;
	}
}