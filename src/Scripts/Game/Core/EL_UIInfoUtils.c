class EL_UIInfoUtils
{
	protected static ref map<ResourceName, ref UIInfo> s_mCache = new map<ResourceName, ref UIInfo>();

	//------------------------------------------------------------------------------------------------
	static UIInfo GetInfo(ResourceName prefab)
	{
		UIInfo resultInfo = s_mCache.Get(prefab);

		if (!resultInfo)
		{
			IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(Resource.Load(prefab));
			if (entitySource)
			{
			    for(int nComponent, componentCount = entitySource.GetComponentCount(); nComponent < componentCount; nComponent++)
			    {
			        IEntityComponentSource componentSource = entitySource.GetComponent(nComponent);
			        if(componentSource.GetClassName().ToType().IsInherited(InventoryItemComponent))
			        {
			            BaseContainer attributesContainer = componentSource.GetObject("Attributes");
			            if (attributesContainer)
			            {
			                BaseContainer itemDisplayNameContainer = attributesContainer.GetObject("ItemDisplayName");
			                if (itemDisplayNameContainer)
			                {
			                    resultInfo = UIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(itemDisplayNameContainer));
			                    break;
			                }
			            }
			        }
			    }
			}

			s_mCache.Set(prefab, resultInfo);
		}

		return resultInfo;
	}

	//------------------------------------------------------------------------------------------------
	static SCR_EditableVehicleUIInfo GetVehicleInfo(ResourceName prefab)
	{
		BaseContainer vehicleUIInfoComponent = SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), "SCR_EditableVehicleComponent");
		if (!vehicleUIInfoComponent)
			return null;

		SCR_EditableVehicleUIInfo vehicleUIInfo;
		vehicleUIInfoComponent.Get("m_UIInfo", vehicleUIInfo);
		return vehicleUIInfo;
	}

	//------------------------------------------------------------------------------------------------
	static UIInfo GetInfo(IEntity entity)
	{
		if (!entity) return null;

		InventoryItemComponent item = EL_ComponentFinder<InventoryItemComponent>.Find(entity);
		if (!item) return null;

		UIInfo resultInfo = item.GetAttributes().GetUIInfo();
		string prefab = EL_Utils.GetPrefabName(entity);
		if (prefab) s_mCache.Set(prefab, resultInfo);

		return resultInfo;
	}
}
