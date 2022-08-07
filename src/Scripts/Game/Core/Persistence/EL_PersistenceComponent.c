class EL_PersistenceComponentClass : ScriptComponentClass
{
	[Attribute(defvalue: "1", desc: "If enabled the entity will spawn back into the world automatically after session restart. Always true for baked map objects.")]
	bool m_bSelfSpawn;
	
	[Attribute(defvalue: "1", desc: "If enabled the entity will deleted from persistence when deleted from the world automatically.")]
	bool m_bSelfDelete;
	
	[Attribute(defvalue: "1", desc: "Choose if this entity is included in the auto save cycle. If disabled you are responsible to call Save() manually.")]
	bool m_bAutosave;
	
	[Attribute(desc: "Type of save-data to represent this entity.")]
	ref EL_EntitySaveDataBase m_pSaveData;
	
	// Derived from inital engine-created instance
	typename m_tSaveDataTypename;
	ref array<typename> m_aComponentSaveDataTypenames;
}

class EL_PersistenceComponent : ScriptComponent
{
	protected bool m_bBaked;
	protected bool m_bStorageRoot;
	protected bool m_bSavedAsStorageRoot;
	protected string m_sId;
	protected DateTimeUtcAsInt m_iLastSaved;
	
	string GetPersistentId()
	{
		return m_sId;
	}
	
	DateTimeUtcAsInt GetLastSaved()
	{
		return m_iLastSaved;
	}
	
	EL_EntitySaveDataBase Save()
	{
		if(!m_sId) return null;
		
		IEntity owner = GetOwner();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		
		EL_EntitySaveDataBase saveData = EL_EntitySaveDataBase.Cast(settings.m_tSaveDataTypename.Spawn());		
		if(!saveData || !saveData.ReadFrom(owner))
		{
			Debug.Error(string.Format("Failed to persist world entity '%1'@%2. Save-data could not be read.", 
				EL_Utils.GetPrefabName(owner), 
				owner.GetOrigin()));
			
			return null;
		}
		
		m_iLastSaved = EL_Utils.GetCurrentUtcAsInt();
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
		if(m_bStorageRoot)
		{
			persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
			
			if(!m_bSavedAsStorageRoot)
			{
				if(settings.m_bSelfSpawn || m_bBaked)
				{
					GetGame().GetScriptModule().Call(persistenceManager, "RegisterRootEntity", false, null, settings.m_tSaveDataTypename, m_sId, m_bBaked, true);
				}
				
				m_bSavedAsStorageRoot = true;
			}
		}
		else if(m_bSavedAsStorageRoot)
		{
			// Was previously saved as storage root but now is not anymore, so the toplevel db entry has to be deleted.
			// The save data will be present inside the storage parent instead.
			Delete();
			m_bSavedAsStorageRoot = false;
		}

		return saveData;
	}
	
	override event void OnPostInit(IEntity owner)
	{
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		
		// Persistence logic only runs on the server
		if(!EL_PersistenceManager.IsPersistenceMaster())
		{
			// Free memory
			settings.m_pSaveData = null;
			Deactivate(owner);
			return;
		}

		// Cache saveData typename on shared instance. We do not need the object instance after that.
		if(!settings.m_tSaveDataTypename)
		{
			if(!settings.m_pSaveData || settings.m_pSaveData.Type() == EL_EntitySaveDataBase)
			{
				Debug.Error(string.Format("Missing or invalid save-data type in persistence component on entity '%1'@%2. Entity will not be persisted!", 
					EL_Utils.GetPrefabName(owner), 
					owner.GetOrigin()));
				
				Deactivate(owner);
				return;
			}
			
			settings.m_tSaveDataTypename = settings.m_pSaveData.Type();
			
			array<typename> componentSaveDataTypes();
			foreach(EL_ComponentSaveDataBase componentSaveData : settings.m_pSaveData.m_aComponents)
			{
				typename componentSaveDataType = componentSaveData.Type();
				
				if(componentSaveDataType == EL_ComponentSaveDataBase)
				{
					Debug.Error(string.Format("Invalid save-data type '%1' in persistence component on entity '%2'@%3. Associated component data will be persisted!", 
						EL_ComponentSaveDataBase,	
						EL_Utils.GetPrefabName(owner), 
						owner.GetOrigin()));
					
					continue;
				}
				
				// We only need the type once, but can't use a set as we need ordering in the step below
				if(componentSaveDataTypes.Contains(componentSaveDataType)) continue;
				
				componentSaveDataTypes.Insert(componentSaveDataType);
			}

			settings.m_aComponentSaveDataTypenames = EL_Utils.SortTypenameHierarchy(componentSaveDataTypes);
			
			// Free memory on shared component data instance
			settings.m_pSaveData = null;
		}
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		
		GetGame().GetScriptModule().Call(persistenceManager, "GeneratePersistentId", false, m_sId, owner);
		if(!m_sId)
		{
			// In case we were unable to get an id (most likely due to invalid data e.g. no name on baked entity) this entity will not be persisted.
			Deactivate(owner);
			return;
		}
		
		m_bStorageRoot = true;

		m_bBaked = persistenceManager.GetState() == EL_EPersistenceManagerState.WORLD_INIT;

		if(settings.m_bAutosave)
		{
			GetGame().GetScriptModule().Call(persistenceManager, "SubscribeAutoSave", false, null, owner);
		}
	}
	
	event void OnStorageParentChanged(IEntity owner, IEntity storageParent)
	{
		if(!IsActive()) return;
		
		m_bStorageRoot = !storageParent;
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		
		// Unscubscribe for when no longer storage root
		// Also need to unsubscribe just in case it was subscribed already, as scriptinvoker will call autosave multiple times otherwise.
		GetGame().GetScriptModule().Call(persistenceManager, "UnsubscribeAutoSave", false, null, owner);

		if(m_bStorageRoot && settings.m_bAutosave)
		{
			// Entity is storage root and needs to be subscribed to auto-save
			GetGame().GetScriptModule().Call(persistenceManager, "SubscribeAutoSave", false, null, owner);
		}
	}
	
	override event void OnDelete(IEntity owner)
    {
		Delete();
    }
	
	void Delete()
	{
		if(!IsActive()) return;
		
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if(!persistenceManager || !persistenceManager.IsActive()) return;
		
		GetGame().GetScriptModule().Call(persistenceManager, "UnsubscribeAutoSave", false, null, GetOwner());
		
		if(!m_bSavedAsStorageRoot) return;
		
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(GetOwner()));

		if(settings.m_bSelfSpawn || m_bBaked)
		{
			GetGame().GetScriptModule().Call(persistenceManager, "UnregisterRootEntity", false, null, settings.m_tSaveDataTypename, m_sId, m_bBaked, true);
		}
		
		// Delete save-data if entity is removed during active gameplay and not session dtor
		if(settings.m_bSelfDelete && m_sId)
		{
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataTypename, m_sId);
		}
	}
	
	#ifdef WORKBENCH
	override event void _WB_OnInit(IEntity owner, inout vector mat[4], IEntitySource src)
	{
		super._WB_OnInit(owner, mat, src);

		if(owner.GetName()) return;

		WorldEditorAPI worldEditorApi = GenericEntity.Cast(owner)._WB_GetEditorAPI();
		if(!worldEditorApi) return;

		string prefabNameOnly = FilePath.StripExtension(FilePath.StripPath(EL_Utils.GetPrefabName(owner)));
		string uuid = Workbench.GenerateGloballyUniqueID64();
		
		worldEditorApi.BeginEntityAction("Fix persistent baked entity name");
		worldEditorApi.RenameEntity(owner, string.Format("%1_%2", prefabNameOnly, uuid));
		worldEditorApi.EndEntityAction("Fix persistent baked entity name");
	}
	#endif
}
