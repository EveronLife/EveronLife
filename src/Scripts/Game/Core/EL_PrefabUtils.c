class EL_PrefabUtils
{
	//------------------------------------------------------------------------------------------------
	//! Gets prefab iventory item weight
	//! \param prefab Prefab path
	//! \return the weight of the prefab iventory item
	static float GetPrefabItemWeight(ResourceName prefab)
	{
		IEntityComponentSource itemComponentSource = SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), InventoryItemComponent);
		if (!itemComponentSource)
			return -1;

		SCR_ItemAttributeCollection itemAttributeCol;
		BaseContainer attributesContainer = itemComponentSource.GetObject("Attributes");
		BaseContainer itemPhysAttributes = attributesContainer.GetObject("ItemPhysAttributes");

		float itemWeight;
		itemPhysAttributes.Get("Weight", itemWeight);
		return itemWeight;
	}	
	
	//------------------------------------------------------------------------------------------------
	//! Gets prefab iventory item volume
	//! \param prefab Prefab path
	//! \return the volume of the prefab iventory item
	static float GetPrefabItemVolume(ResourceName prefab)
	{
		IEntityComponentSource itemComponentSource = SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), InventoryItemComponent);
		if (!itemComponentSource)
			return -1;

		SCR_ItemAttributeCollection itemAttributeCol;
		BaseContainer attributesContainer = itemComponentSource.GetObject("Attributes");
		BaseContainer itemPhysAttributes = attributesContainer.GetObject("ItemPhysAttributes");
		
		float itemVolume;
		itemPhysAttributes.Get("ItemVolume", itemVolume);
		return itemVolume;
	}
	
	
	//------------------------------------------------------------------------------------------------
	//! Gets prefab VObject
	//! \param prefab Prefab path
	//! \return the VObject of the prefab
	static VObject GetPrefabVObject(ResourceName prefab)
	{
		IEntityComponentSource itemComponentSource = SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), "MeshObject");
		if (!itemComponentSource)
			return null;
		ResourceName resourceNameObject;
		itemComponentSource.Get("Object", resourceNameObject);
		return Resource.Load(resourceNameObject).GetResource().ToVObject();
	}
}
