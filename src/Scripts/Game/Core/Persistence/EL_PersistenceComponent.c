[ComponentEditorProps(category: "EveronLife/Core/Persistence", description: "Used to make an entity persistent.")]
class EL_PersistenceComponentClass : ScriptComponentClass
{
	[Attribute(defvalue: "1", desc: "Only storage root entities can be saved in the open world.\nIf disabled the entity will only be saved if inside another storage root (e.g. character, vehicle).")]
	bool m_bStorageRoot;

	[Attribute(defvalue: "1", desc: "Regulary save this entity during active session.\nThe interval is configured in the persitence manager component on your game mode.")]
	bool m_bAutosave;

	[Attribute(defvalue: "1", desc: "Save entity on session shutdown.\nOnly disable this, if you know what you are doing!")]
	bool m_bShutdownsave;

	[Attribute(defvalue: "1", desc: "If enabled the entity will spawn back into the world automatically after session restart.\nAlways true for baked map objects.")]
	bool m_bSelfSpawn;

	[Attribute(defvalue: "1", desc: "If enabled the entity will deleted from persistence when deleted from the world automatically.")]
	bool m_bSelfDelete;

	[Attribute(desc: "Type of save-data to represent this entity.")]
	ref EL_EntitySaveDataClass m_pSaveData;

	// Derived from shared initialization
	typename m_tSaveDataTypename;

	//------------------------------------------------------------------------------------------------
	static override bool DependsOn(string className)
	{
		return true; // Forcing persistence to be loaded last so other components are properly initalized to be read from and applied to.
	}
}

class EL_PersistenceComponent : ScriptComponent
{
	protected string m_sId;
	protected EL_DateTimeUtcAsInt m_iLastSaved;

	protected bool m_bStorageRootState;
	protected bool m_bSavedAsStorageRoot;
	protected bool m_bDetatched;

	//------------------------------------------------------------------------------------------------
	//! static helper see GetPersistentId()
	static string GetPersistentId(IEntity worldEntity)
	{
		if (!worldEntity) return string.Empty;
		EL_PersistenceComponent persistence = EL_PersistenceComponent.Cast(worldEntity.FindComponent(EL_PersistenceComponent));
		if (!persistence) return string.Empty;
		return persistence.GetPersistentId();
	}

	//------------------------------------------------------------------------------------------------
	//! Get the assigned persistent id of this entity
	//! \return the id or empty string if persistence data is deleted and only the instance remains
	string GetPersistentId()
	{
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the last time this entity was saved as packed UTC date time
	//! \return utc date time represented as packed integer
	EL_DateTimeUtcAsInt GetLastSaved()
	{
		return m_iLastSaved;
	}

	//------------------------------------------------------------------------------------------------
	//! Set the last time this entity was saved as packed UTC date time.
	//! ONLY USE WHEN YOU KNOW WHAT YOU ARE DOING! PRIMARILY INTERNAL USE.
	void SetLastSaved(EL_DateTimeUtcAsInt dateTime)
	{
		m_iLastSaved = dateTime;
	}

	//------------------------------------------------------------------------------------------------
	//! Save the entity to the database
	//! \return the save-data instance that was submitted to the database
	EL_EntitySaveData Save()
	{
		if (m_bDetatched || !m_sId) return null;

		m_iLastSaved = EL_DateTimeUtcAsInt.Now();

		IEntity owner = GetOwner();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		EL_EntitySaveData saveData = EL_EntitySaveData.Cast(settings.m_tSaveDataTypename.Spawn());
		if (!saveData || !saveData.ReadFrom(owner, settings.m_pSaveData))
		{
			Debug.Error(string.Format("Failed to persist world entity '%1'@%2. Save-data could not be read.",
				EL_Utils.GetPrefabName(owner),
				owner.GetOrigin()));
			return null;
		}

		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();

		// Ignore "root" entities if they are stored inside others until we have access to that event properly in script
		if (m_bStorageRootState && !EL_PersistenceUtils.IsAttached(owner))
		{
			persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
			m_bSavedAsStorageRoot = true;
		}
		else if (m_bSavedAsStorageRoot)
		{
			// Was previously saved as storage root but now is not anymore, so the toplevel db entry has to be deleted.
			// The save-data will be present inside the storage parent instead.
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataTypename, GetPersistentId());
			m_bSavedAsStorageRoot = false;
		}

		return saveData;
	}

	//------------------------------------------------------------------------------------------------
	//! Load existing save-data to apply to this entity
	bool Load(notnull EL_EntitySaveData saveData)
	{
		IEntity owner = GetOwner();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		if (m_bDetatched || saveData.GetId() != m_sId || !saveData.ApplyTo(owner, settings.m_pSaveData))
		{
			Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", saveData.Type().ToString(), saveData.GetId()));
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override event void OnPostInit(IEntity owner)
	{
		// Persistence logic only runs on the server
		if (!EL_PersistenceManager.IsPersistenceMaster()) return;

		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));

		// Init and validate settings on shared class-class instance once
		if (!settings.m_tSaveDataTypename)
		{
			if (!settings.m_pSaveData || settings.m_pSaveData.Type() == EL_EntitySaveDataClass)
			{
				Debug.Error(string.Format("Missing or invalid save-data type in persistence component on entity '%1'@%2. Entity will not be persisted!",
					EL_Utils.GetPrefabName(owner),
					owner.GetOrigin()));
				return;
			}

			settings.m_tSaveDataTypename = EL_Utils.TrimEnd(settings.m_pSaveData.ClassName(), 5).ToType();

			// Collect and validate component save data types
			array<typename> componentSaveDataTypes();
			componentSaveDataTypes.Reserve(settings.m_pSaveData.m_aComponents.Count());
			foreach (EL_ComponentSaveDataClass componentSaveData : settings.m_pSaveData.m_aComponents)
			{
				typename componentSaveDataType = componentSaveData.Type();

				if (!componentSaveDataType || componentSaveDataType == EL_ComponentSaveDataClass)
				{
					Debug.Error(string.Format("Invalid save-data type '%1' in persistence component on entity '%2'@%3. Associated component data will not be persisted!",
						componentSaveDataType,
						EL_Utils.GetPrefabName(owner),
						owner.GetOrigin()));
					continue;
				}

				componentSaveDataTypes.Insert(componentSaveDataType);
			}

			// Re-order save data class-classes in attribute instance by inheritance
			array<ref EL_ComponentSaveDataClass> sortedComponents();
			sortedComponents.Reserve(settings.m_pSaveData.m_aComponents.Count());
			foreach (typename componentType : EL_Utils.SortTypenameHierarchy(componentSaveDataTypes))
			{
				foreach (EL_ComponentSaveDataClass componentSaveData : settings.m_pSaveData.m_aComponents)
				{
					if (componentSaveData.Type() == componentType) sortedComponents.Insert(componentSaveData);
				}
			}
			settings.m_pSaveData.m_aComponents = sortedComponents;
		}

		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();

		persistenceManager.RequestPersistentId(this);

		if (settings.m_bStorageRoot)
		{
			persistenceManager.RegisterSaveRoot(this, settings.m_bAutosave);
			m_bStorageRootState = true;
		}
	}

	//------------------------------------------------------------------------------------------------
	event void OnPersistentIdAssigned(string persistentId)
	{
		m_sId = persistentId;
	}

	//------------------------------------------------------------------------------------------------
	event void OnStorageParentChanged(IEntity owner, IEntity storageParent)
	{
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));

		// If not currently tracked by persistence ignore changes.
		// This can happen on delete from inventory manager where the event comes after OnDelete of this component
		if (m_bDetatched || !m_sId || !settings.m_bStorageRoot) return;

		if (m_bStorageRootState && storageParent)
		{
			// Entity was previously the save root, but now got a parent assigned, so unregister
			persistenceManager.UnregisterSaveRoot(this);
			m_bStorageRootState = false;
		}
		else if (!m_bStorageRootState && !storageParent)
		{
			// Entity was previously a save child, but now has no parent anymore and thus needs to be registerd as own root
			persistenceManager.RegisterSaveRoot(this, settings.m_bAutosave);
			m_bStorageRootState = true;
		}
	}

	//------------------------------------------------------------------------------------------------
	override event void OnDelete(IEntity owner)
	{
		// Check that we are not in session dtor phase
		EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance(false);
		if (!persistenceManager || (persistenceManager.GetState() == EL_EPersistenceManagerState.SHUTDOWN)) return;
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));

		if (settings.m_bStorageRoot)
		{
			persistenceManager.UnregisterSaveRoot(this);
		}

		persistenceManager.UnloadPersistentId(m_sId);

		// Only auto self delete if setting for it is enabled
		if (!settings.m_bSelfDelete) return;

		Delete();
	}

	//------------------------------------------------------------------------------------------------
	//! Delete the persistence data of this entity. Does not delete the entity itself.
	void Delete()
	{
		if (m_bDetatched || !m_sId) return;

		if (m_bSavedAsStorageRoot)
		{
			// Only attempt to delete if there is a chance it was already saved as own entity in db
			EL_PersistenceManagerInternal persistenceManager = EL_PersistenceManagerInternal.GetInternalInstance();
			EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(GetOwner()));
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataTypename, m_sId);
		}

		m_sId = string.Empty;
		m_iLastSaved = 0;
	}

	//------------------------------------------------------------------------------------------------
	//! Mark the entity as detached from persistence, to ignore Save, Load and Delete operations. Can not be undone. Used primarily to handle removal of the instance externally.
	void Detach()
	{
		m_bDetatched = true;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the instance of the save-data or component save-data class-class that was configured in the prefab/world editor.
	//! \param saveDataClassType typename of the entity or component save-data which should be returned
	//! \return instance of the save-data or null if not found
	Class GetAttributeClass(typename saveDataClassType)
	{
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(GetOwner()));
		if (settings.m_pSaveData) return null;

		if (settings.m_pSaveData.IsInherited(saveDataClassType))
		{
			return settings.m_pSaveData;
		}
		else
		{
			// Find the first inheritance match. The typename array itereated is ordered by inheritance.
			foreach (EL_ComponentSaveDataClass componentSaveDataClass : settings.m_pSaveData.m_aComponents)
			{
				if (componentSaveDataClass.IsInherited(saveDataClassType)) return componentSaveDataClass;
			}
		}

		return null;
	}

	//------------------------------------------------------------------------------------------------
	#ifdef WORKBENCH
	override event void _WB_OnInit(IEntity owner, inout vector mat[4], IEntitySource src)
	{
		super._WB_OnInit(owner, mat, src);

		if (owner.GetName()) return;

		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
		if (!worldEditor || worldEditor.IsPrefabEditMode()) return;

		WorldEditorAPI worldEditorApi = GenericEntity.Cast(owner)._WB_GetEditorAPI();
		if (!worldEditorApi) return;

		string prefabNameOnly = FilePath.StripExtension(FilePath.StripPath(EL_Utils.GetPrefabName(owner)));
		if (!prefabNameOnly) prefabNameOnly = owner.ClassName();

		string uuid = Workbench.GenerateGloballyUniqueID64();
		worldEditorApi.RenameEntity(owner, string.Format("%1_%2", prefabNameOnly, uuid));
	}
	#endif
}
