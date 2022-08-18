class EL_PersistenceComponentClass : ScriptComponentClass
{
	[Attribute(defvalue: "1", desc: "Regulary save this entity during active session. The interval is configured in the persitence manager component on your game mode.")]
	bool m_bAutosave;
	
	[Attribute(defvalue: "1", desc: "Save entity on session shutdown. Only disable this, if you know what you are doing!")]
	bool m_bShutdownsave;
	
	[Attribute(defvalue: "1", desc: "If enabled the entity will spawn back into the world automatically after session restart. Always true for baked map objects.")]
	bool m_bSelfSpawn;
	
	[Attribute(defvalue: "1", desc: "If enabled the entity will deleted from persistence when deleted from the world automatically.")]
	bool m_bSelfDelete;
	
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
	protected EL_DateTimeUtcAsInt m_iLastSaved;
	
	// Used for deferred loading during world init 
	protected ref EL_EntitySaveDataBase m_pSaveDataBuffer;
	
	string GetPersistentId()
	{
		return m_sId;
	}
	
	EL_DateTimeUtcAsInt GetLastSaved()
	{
		return m_iLastSaved;
	}
	
	EL_EntitySaveDataBase Save()
	{
		if(!IsActive()) return null;
		
		IEntity owner = GetOwner();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		
		m_iLastSaved = EL_DateTimeUtcAsInt.Now();
		
		EL_EntitySaveDataBase saveData = EL_EntitySaveDataBase.Cast(settings.m_tSaveDataTypename.Spawn());		
		if(!saveData || !saveData.ReadFrom(owner))
		{
			Debug.Error(string.Format("Failed to persist world entity '%1'@%2. Save-data could not be read.", 
				EL_Utils.GetPrefabName(owner), 
				owner.GetOrigin()));
			
			return null;
		}
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		
		if(m_bStorageRoot)
		{
			persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
			m_bSavedAsStorageRoot = true;
		}
		else if(m_bSavedAsStorageRoot)
		{
			// Was previously saved as storage root but now is not anymore, so the toplevel db entry has to be deleted.
			// The save data will be present inside the storage parent instead.	
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataTypename, m_sId);
			m_bSavedAsStorageRoot = false;
		}
		
		return saveData;
	}
	
	override event void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	override protected void EOnInit(IEntity owner)
	{
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		
		// Persistence logic only runs on the server
		if(!EL_PersistenceManager.IsPersistenceMaster())
		{
			// Free memory
			settings.m_pSaveData = null;
			return;
		}
		
		// Cache save-data typename on shared instance. We do not need the object instance after that.
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
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		
		if(persistenceManager.GetState() == EL_EPersistenceManagerState.WORLD_INIT)
		{
			m_sId = persistenceManager.GetPersistentId(owner);
			m_bBaked = true;
		}
		
		EL_EntitySaveDataBase saveData = persistenceManager.GetEntitySaveDataBuffer(m_sId);
		if(saveData)
		{
			if(m_bBaked)
			{
				// Save the data for world post init
				m_pSaveDataBuffer = saveData;
			}
			else if(!saveData.ApplyTo(owner))
			{
				Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", saveData.Type(), saveData.GetId()));
				Deactivate(owner);
				return;
			}
			
			m_sId = saveData.GetId();
			m_iLastSaved = saveData.m_iLastSaved;
		}
		else if(!m_sId) // Non baked entities without save data need a new one generated
		{
			m_sId = persistenceManager.GeneratePersistentId();
		}
		
		m_bStorageRoot = true;
		
		typename selfSpawnType = typename.Empty;
		if(settings.m_bSelfSpawn) selfSpawnType = settings.m_tSaveDataTypename;
		persistenceManager.RegisterSaveRoot(this, m_bBaked, selfSpawnType, settings.m_bAutosave);
	}
	
	event void OnWorldPostProcess(IEntity owner)
	{
		if(!m_pSaveDataBuffer) return;
		
		if(m_pSaveDataBuffer.ApplyTo(owner))
		{
			m_pSaveDataBuffer = null;
			return;
		}
		
		Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", m_pSaveDataBuffer.Type(), m_pSaveDataBuffer.GetId()));
		Deactivate(owner);
	}
	
	event void OnStorageParentChanged(IEntity owner, IEntity storageParent)
	{
		if(!IsActive()) return;
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		
		typename selfSpawnType = typename.Empty;
		if(settings.m_bSelfSpawn) selfSpawnType = settings.m_tSaveDataTypename;
		
		if(m_bStorageRoot && storageParent)
		{
			// Entity was previously the save root, but now got a parent assigned, so unregister
			persistenceManager.UnregisterSaveRoot(this, m_bBaked, selfSpawnType);
			m_bStorageRoot = false;
		}
		else if(!m_bStorageRoot && !storageParent)
		{
			// Entity was previously a save child, but now has no parent anymore and thus needs to be registerd as own root
			persistenceManager.RegisterSaveRoot(this, m_bBaked, selfSpawnType, settings.m_bAutosave);
			m_bStorageRoot = true;
		}
	}
	
	override event void OnDelete(IEntity owner)
    {
		// Check that we are not in session dtor phase
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if(!persistenceManager || !persistenceManager.IsActive()) return;
		
		// Only auto self delete if setting for it is enabled
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		if(!settings.m_bSelfDelete) return;
		
		Delete();
    }
	
	void Delete()
	{
		if(!IsActive()) return;
		
		IEntity owner = GetOwner();
		
		// Once deleted this world entity becomes inactive in terms of persistence
		Deactivate(owner);
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		
		typename selfSpawnType = typename.Empty;
		if(settings.m_bSelfSpawn) selfSpawnType = settings.m_tSaveDataTypename;
		persistenceManager.UnregisterSaveRoot(this, m_bBaked, selfSpawnType);
		
		if (m_bSavedAsStorageRoot)
		{
			// Only attempt to delete if there is a chance it was already saved as own entity in db
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataTypename, m_sId);
		}
		
		m_sId = string.Empty;
		m_iLastSaved = 0;
	}
		
	#ifdef WORKBENCH
	override event void _WB_OnInit(IEntity owner, inout vector mat[4], IEntitySource src)
	{
		super._WB_OnInit(owner, mat, src);

		if(owner.GetName()) return;

		WorldEditorAPI worldEditorApi = GenericEntity.Cast(owner)._WB_GetEditorAPI();
		if(!worldEditorApi) return;

		string worldPath;
		worldEditorApi.GetWorldPath(worldPath);
		if(worldPath.IsEmpty()) return;
		
		string prefabNameOnly = FilePath.StripExtension(FilePath.StripPath(EL_Utils.GetPrefabName(owner)));
		string uuid = Workbench.GenerateGloballyUniqueID64();
		worldEditorApi.RenameEntity(owner, string.Format("%1_%2", prefabNameOnly, uuid));
	}
	#endif
}
