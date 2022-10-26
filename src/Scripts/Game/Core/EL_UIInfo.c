class EL_UIInfo
{
	protected static ref map<ResourceName, ref EL_UIInfo> s_mCache = new map<ResourceName, ref EL_UIInfo>();

	protected string m_sName;
	protected string m_sDescription;
	protected ResourceName m_rIconPath;

	//------------------------------------------------------------------------------------------------
	string GetName()
	{
		return m_sName;
	}

	//------------------------------------------------------------------------------------------------
	string GetDescription()
	{
		return m_sDescription;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetIconPath()
	{
		return m_rIconPath;
	}

	//------------------------------------------------------------------------------------------------
	static EL_UIInfo FromPrefab(ResourceName prefab)
	{
		EL_UIInfo resultInfo = s_mCache.Get(prefab);

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
						// TODO: Access directly and return UIInfo instead of scripted wrapper once https://feedback.bistudio.com/T167392 is fixed. 
						//		 Just "typedef UIInfo EL_UIInfo" then to make it backwardscompatible.
						BaseContainer attributesContainer = componentSource.GetObject("Attributes");
						if (attributesContainer)
						{
							BaseContainer itemDisplayNameContainer = attributesContainer.GetObject("ItemDisplayName");
							if (itemDisplayNameContainer)
							{
								string name, description;
								ResourceName icon;
								itemDisplayNameContainer.Get("Name", name);
								itemDisplayNameContainer.Get("Description", description);
								itemDisplayNameContainer.Get("Icon", icon);

								resultInfo = new EL_UIInfo(name, description, icon);
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
	static EL_UIInfo FromEntity(IEntity entity)
	{
		if (!entity) return null;

		InventoryItemComponent item = InventoryItemComponent.Cast(entity.FindComponent(InventoryItemComponent));
		if (!item) return null;

		EL_UIInfo resultInfo = FromUIInfo(item.GetAttributes().GetUIInfo());
		string prefab = EL_Utils.GetPrefabName(entity);
		if (prefab) s_mCache.Set(prefab, resultInfo);

		return resultInfo;
	}

	//------------------------------------------------------------------------------------------------
	static EL_UIInfo FromUIInfo(UIInfo uiInfo)
	{
		if (!uiInfo) return null;
		return new EL_UIInfo(uiInfo.GetName(), uiInfo.GetDescription(), uiInfo.GetIconPath());
	}

	//------------------------------------------------------------------------------------------------
	void EL_UIInfo(string name, string description, ResourceName iconPath)
	{
		m_sName = name;
		m_sDescription = description;
		m_rIconPath = iconPath;
	}
}
