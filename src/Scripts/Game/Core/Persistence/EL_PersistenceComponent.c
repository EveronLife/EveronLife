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
	
	static string GetPersistentId(IEntity worldEntity)
	{
		if(!worldEntity) return string.Empty;
		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		if(!persistence) return string.Empty;
		return persistence.GetPersistentId();
	}
	
	string GetPersistentId()
	{
		if(!m_sId) m_sId = EL_PersistenceManagerInternal.GetInternalInstance().GetPersistentId(this);
		
		return m_sId;
	}
	
	EL_DateTimeUtcAsInt GetLastSaved()
	{
		return m_iLastSaved;
	}
	
	EL_EntitySaveDataBase Save()
	{
		if(!GetPersistentId()) return null;
		
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
		
		// Ignore "root" entities if they are stored inside others until we have access to that event properly in script
		bool storageRoot = m_bStorageRoot;
		InventoryItemComponent inventoryItemComponent = InventoryItemComponent.Cast(GetOwner().FindComponent(InventoryItemComponent));
		if(storageRoot && inventoryItemComponent && inventoryItemComponent.GetParentSlot())
		{
			storageRoot = false;
		}
		
		if(storageRoot)
		{
			persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
			m_bSavedAsStorageRoot = true;
		}
		else if(m_bSavedAsStorageRoot)
		{
			// Was previously saved as storage root but now is not anymore, so the toplevel db entry has to be deleted.
			// The save data will be present inside the storage parent instead.	
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataTypename, GetPersistentId());
			m_bSavedAsStorageRoot = false;
		}
		
		return saveData;
	}
	
	bool Load(notnull EL_EntitySaveDataBase saveData)
	{
		if(!saveData.GetId() || !saveData.ApplyTo(GetOwner()))
		{
			Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", saveData.Type().ToString(), saveData.GetId()));
			return false;
		}
		
		m_sId = saveData.GetId();
		m_iLastSaved = saveData.m_iLastSaved;
		
		return true;
	}
	
	override event void OnPostInit(IEntity owner)
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
		
		m_sId = persistenceManager.GetPersistentId(this);
		if(!m_sId) return;
		
		m_bBaked = persistenceManager.GetState() == EL_EPersistenceManagerState.WORLD_INIT;
		
		persistenceManager.RegisterSaveRoot(this, m_bBaked, settings.m_bAutosave);
		m_bStorageRoot = true;
	}
	
	event void OnStorageParentChanged(IEntity owner, IEntity storageParent)
	{
		// If not currently tracked by persistence ignore changes.
		// This can happen on delete from inventory manager where the event comes after OnDelete of this component
		if(!m_sId) return;
		
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		
		if(m_bStorageRoot && storageParent)
		{
			// Entity was previously the save root, but now got a parent assigned, so unregister
			persistenceManager.UnregisterSaveRoot(this, m_bBaked);
			m_bStorageRoot = false;
		}
		else if(!m_bStorageRoot && !storageParent)
		{
			// Entity was previously a save child, but now has no parent anymore and thus needs to be registerd as own root
			EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
			persistenceManager.RegisterSaveRoot(this, m_bBaked, settings.m_bAutosave);
			m_bStorageRoot = true;
		}
	}
	
	override event void OnDelete(IEntity owner)
    {
		// Check that we are not in session dtor phase
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		if(!persistenceManager || (persistenceManager.GetState() == EL_EPersistenceManagerState.SHUTDOWN)) return;
		
		persistenceManager.UnloadPersistentId(m_sId);
		
		// Only auto self delete if setting for it is enabled
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		if(!settings.m_bSelfDelete) return;
		
		Delete();
    }
	
	void Delete()
	{
		if(!m_sId) return;
		
		IEntity owner = GetOwner();

		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();

		persistenceManager.UnregisterSaveRoot(this, m_bBaked);
		
		if (m_bSavedAsStorageRoot)
		{
			// Only attempt to delete if there is a chance it was already saved as own entity in db
			EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
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

		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
		if(!worldEditor || worldEditor.IsPrefabEditMode()) return;
		
		WorldEditorAPI worldEditorApi = GenericEntity.Cast(owner)._WB_GetEditorAPI();
		if(!worldEditorApi) return;
		
		string prefabNameOnly = FilePath.StripExtension(FilePath.StripPath(EL_Utils.GetPrefabName(owner)));
		string uuid = Workbench.GenerateGloballyUniqueID64();
		worldEditorApi.RenameEntity(owner, string.Format("%1_%2", prefabNameOnly, uuid));
	}
	#endif
}
