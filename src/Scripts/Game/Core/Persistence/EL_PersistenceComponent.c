[ComponentEditorProps(category: "EveronLife/Core/Persistence", description: "Used to make an entity persistent.")]
class EL_PersistenceComponentClass : ScriptComponentClass
{
	[Attribute(defvalue: "2", uiwidget: UIWidgets.ComboBox, desc: "Should the entity be saved automatically and if so only on shutdown or regulary.\nThe interval is configured in the persitence manager component on your game mode.", enums: ParamEnumArray.FromEnum(EL_ESaveType))]
	EL_ESaveType m_eSaveType;

	[Attribute(defvalue: "1", desc: "If enabled the entity will spawn back into the world automatically after session restart.\nAlways true for baked map objects.")]
	bool m_bSelfSpawn;

	[Attribute(defvalue: "1", desc: "If enabled the entity will deleted from persistence when deleted from the world automatically.")]
	bool m_bSelfDelete;

	[Attribute(defvalue: "1", desc: "Only storage root entities can be saved in the open world.\nIf disabled the entity will only be saved if inside another storage root (e.g. character, vehicle).")]
	bool m_bStorageRoot;

	[Attribute(desc: "Type of save-data to represent this entity.")]
	ref EL_EntitySaveDataClass m_pSaveData;

	// Derived from shared initialization
	typename m_tSaveDataTypename;

	//------------------------------------------------------------------------------------------------
	static override bool DependsOn(string className)
	{
		return true; // Forcing persistence to be loaded last so other components are properly initalized to be read from and applied to.
	}

	//------------------------------------------------------------------------------------------------
	static override array<typename> CannotCombine(IEntityComponentSource src)
	{
		return {EL_PersistenceComponent}; //Prevent multiple persistence components from being added.
	}
}

class EL_PersistenceComponent : ScriptComponent
{
	private string m_sId;
	private EL_DateTimeUtcAsInt m_iLastSaved;
	[NonSerialized()]
	private EL_EPersistenceFlags m_eFlags;

	[NonSerialized()]
	private ref ScriptInvoker<EL_PersistenceComponent, EL_EntitySaveData> m_pOnAfterSave;
	[NonSerialized()]
	private ref ScriptInvoker<EL_PersistenceComponent, EL_EntitySaveData> m_pOnAfterPersist;
	[NonSerialized()]
	private ref ScriptInvoker<EL_PersistenceComponent, EL_EntitySaveData> m_pOnBeforeLoad;
	[NonSerialized()]
	private ref ScriptInvoker<EL_PersistenceComponent, EL_EntitySaveData> m_pOnAfterLoad;

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
	//! Get the assigned persistent id of this entity.
	string GetPersistentId()
	{
		if (!m_sId) m_sId = EL_PersistenceManager.GetInstance().Register(this);
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Set the assigned persistent id of this entity.
	//! USE WITH CAUTION! Only in rare situations you need to manually assign an id.
	void SetPersistentId(string id)
	{
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if (m_sId && m_sId != id)
		{
			persistenceManager.Unregister(this);
			m_sId = string.Empty;
		}
		if (!m_sId) m_sId = persistenceManager.Register(this, id);
	}

	//------------------------------------------------------------------------------------------------
	//! Get the last time this entity was saved as packed UTC date time
	//! \return utc date time represented as packed integer
	EL_DateTimeUtcAsInt GetLastSaved()
	{
		return m_iLastSaved;
	}

	//------------------------------------------------------------------------------------------------
	//! Get internal state flags of the persistence tracking
	EL_EPersistenceFlags GetFlags()
	{
		return m_eFlags;
	}

	//------------------------------------------------------------------------------------------------
	//! Event invoker for when the save-data was created but was not yet persisted to the database.
	//! Args(EL_PersistenceComponent, EL_EntitySaveData)
	ScriptInvoker GetOnAfterSaveEvent()
	{
		if (!m_pOnAfterSave) m_pOnAfterSave = new ScriptInvoker();
		return m_pOnAfterSave;
	}

	//------------------------------------------------------------------------------------------------
	//! Event invoker for when the save-data was persisted to the database.
	//! Only called on world root entities (e.g. not on items stored inside other items, there it will only be called for the container).
	//! Args(EL_PersistenceComponent, EL_EntitySaveData)
	ScriptInvoker GetOnAfterPersistEvent()
	{
		if (!m_pOnAfterPersist) m_pOnAfterPersist = new ScriptInvoker();
		return m_pOnAfterPersist;
	}

	//------------------------------------------------------------------------------------------------
	//! Event invoker for when the save-data is about to be loaded/applied to the entity.
	//! Args(EL_PersistenceComponent, EL_EntitySaveData)
	ScriptInvoker GetOnBeforeLoadEvent()
	{
		if (!m_pOnBeforeLoad) m_pOnBeforeLoad = new ScriptInvoker();
		return m_pOnBeforeLoad;
	}

	//------------------------------------------------------------------------------------------------
	//! Event invoker for when the save-data was loaded/applied to the entity.
	//! Args(EL_PersistenceComponent, EL_EntitySaveData)
	ScriptInvoker GetOnAfterLoadEvent()
	{
		if (!m_pOnAfterLoad) m_pOnAfterLoad = new ScriptInvoker();
		return m_pOnAfterLoad;
	}

	//------------------------------------------------------------------------------------------------
	//! Save the entity to the database
	//! \return the save-data instance that was submitted to the database
	EL_EntitySaveData Save()
	{
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

		if (m_pOnAfterSave) m_pOnAfterSave.Invoke(this, saveData);

		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();

		// Ignore "root" entities if they are stored inside others
		if (EL_BitFlags.CheckFlags(m_eFlags, EL_EPersistenceFlags.STORAGE_ROOT))
		{
			persistenceManager.GetDbContext().AddOrUpdateAsync(saveData);
			EL_BitFlags.SetFlags(m_eFlags, EL_EPersistenceFlags.STORAGE_ROOT_SAVED);
			if (m_pOnAfterPersist) m_pOnAfterPersist.Invoke(this, saveData);
		}
		else if (EL_BitFlags.CheckFlags(m_eFlags, EL_EPersistenceFlags.STORAGE_ROOT_SAVED))
		{
			// Was previously saved as storage root but now is not anymore, so the toplevel db entry has to be deleted.
			// The save-data will be present inside the storage parent instead.
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataTypename, GetPersistentId());
			EL_BitFlags.ClearFlags(m_eFlags, EL_EPersistenceFlags.STORAGE_ROOT_SAVED);
		}

		return saveData;
	}

	//------------------------------------------------------------------------------------------------
	//! Load existing save-data to apply to this entity
	bool Load(notnull EL_EntitySaveData saveData)
	{
		if (m_pOnBeforeLoad) m_pOnBeforeLoad.Invoke(this, saveData);

		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
		if (m_sId && m_sId != saveData.GetId())
		{
			persistenceManager.Unregister(this);
			m_sId = string.Empty;
		}
		if (!m_sId) m_sId = persistenceManager.Register(this, saveData.GetId());

		IEntity owner = GetOwner();
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		if (!saveData.ApplyTo(owner, settings.m_pSaveData))
		{
			Debug.Error(string.Format("Failed to apply save-data '%1:%2' to entity.", saveData.Type().ToString(), saveData.GetId()));
			return false;
		}

		if (m_pOnAfterLoad) m_pOnAfterLoad.Invoke(this, saveData);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override event void OnPostInit(IEntity owner)
	{
		// Persistence logic only runs on the server
		if (!EL_PersistenceManager.IsPersistenceMaster()) return;

		// Init and validate settings on shared class-class instance once
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
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

		EL_BitFlags.SetFlags(m_eFlags, EL_EPersistenceFlags.STORAGE_ROOT);
		EL_PersistenceManager.GetInstance().EnqueueForRegistration(this);

		// TODO: Remove afer 0.9.8 and rely only on OnAddedToParent
		InventoryItemComponent invItem = EL_Component<InventoryItemComponent>.Find(owner);
		if (invItem) invItem.m_OnParentSlotChangedInvoker.Insert(OnParentSlotChanged);
	}

	//------------------------------------------------------------------------------------------------
	void OnParentSlotChanged(InventoryStorageSlot oldSlot, InventoryStorageSlot newSlot)
	{
		if (newSlot)
		{
			OnAddedToParentPlaceholder(GetOwner(), newSlot.GetOwner());
		}
		else if (oldSlot)
		{
			OnRemovedFromParentPlaceholder(GetOwner(), oldSlot.GetOwner());
		}
	}

	//------------------------------------------------------------------------------------------------
	/*override*/ event void OnAddedToParentPlaceholder(IEntity child, IEntity parent)
	{
		UpdateRootStatus();
	}

	//------------------------------------------------------------------------------------------------
	/*override*/ event void OnRemovedFromParentPlaceholder(IEntity child, IEntity parent)
	{
		UpdateRootStatus(true);
	}

	//------------------------------------------------------------------------------------------------
	override event void OnDelete(IEntity owner)
	{
		// TODO: Remove afer 0.9.8 and rely only on OnRemovedFromParent
		InventoryItemComponent invItem = EL_Component<InventoryItemComponent>.Find(owner);
		if (invItem) invItem.m_OnParentSlotChangedInvoker.Remove(OnParentSlotChanged);

		// Check that we are not in session dtor phase
		EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance(false);
		if (!persistenceManager || persistenceManager.GetState() == EL_EPersistenceManagerState.SHUTDOWN) return;

		persistenceManager.Unregister(this);

		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(owner));
		if (!m_sId || EL_BitFlags.CheckFlags(m_eFlags, EL_EPersistenceFlags.PAUSE_TRACKING))
		{
			persistenceManager.UpdateRootEntityCollection(this, m_sId, false);
			if (settings.m_bSelfDelete) Delete();
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Delete the persistence data of this entity. Does not delete the entity itself.
	void Delete()
	{
		if (m_sId && EL_BitFlags.CheckFlags(m_eFlags, EL_EPersistenceFlags.STORAGE_ROOT_SAVED))
		{
			// Only attempt to delete if there is a chance it was already saved as own entity in db
			EL_PersistenceManager persistenceManager = EL_PersistenceManager.GetInstance();
			EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(GetOwner()));
			persistenceManager.GetDbContext().RemoveAsync(settings.m_tSaveDataTypename, m_sId);
		}

		m_sId = string.Empty;
		m_iLastSaved = 0;
	}

	//------------------------------------------------------------------------------------------------
	//! Pause automated tracking for auto-/shutdown-save, root entity changes and removal.
	//! Used primarily to handle the conditional removal of an entity manually. E.g. pause before virtually storing a vehicle in a garage (during which the entity gets deleted).
	void PauseTracking()
	{
		EL_BitFlags.SetFlags(m_eFlags, EL_EPersistenceFlags.PAUSE_TRACKING);
	}

	//------------------------------------------------------------------------------------------------
	//! Undo PauseTracking().
	void ResumeTracking()
	{
		EL_BitFlags.ClearFlags(m_eFlags, EL_EPersistenceFlags.PAUSE_TRACKING);
		UpdateRootStatus();
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
	protected void UpdateRootStatus(bool forceRoot = false)
	{
		EL_PersistenceComponentClass settings = EL_PersistenceComponentClass.Cast(GetComponentData(GetOwner()));

		// Only count valid entity link systems as non root
		bool isRoot = settings.m_bStorageRoot && (forceRoot || IsRootEntity());
		if (isRoot)
		{
			EL_BitFlags.SetFlags(m_eFlags, EL_EPersistenceFlags.STORAGE_ROOT);
		}
		else
		{
			EL_BitFlags.ClearFlags(m_eFlags, EL_EPersistenceFlags.STORAGE_ROOT);
		}

		if (m_sId && !EL_BitFlags.CheckFlags(m_eFlags, EL_EPersistenceFlags.PAUSE_TRACKING))
		{
			EL_PersistenceManager.GetInstance().UpdateRootStatus(this, m_sId, settings.m_eSaveType, isRoot);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsRootEntity()
	{
		IEntity child = GetOwner();

		// No parent, but all other systems checked would result in it having a parent so we can stop early
		IEntity parent = child.GetParent();
		if (!parent) return false;

		// If inventory item then return if parent slot is null or not
		InventoryItemComponent inventoryItem = EL_Component<InventoryItemComponent>.Find(child);
		if (inventoryItem && inventoryItem.GetParentSlot()) return false;

		// Check if stored inside any storage on the parent
		array<Managed> outComponents();
		/*parent.FindComponents(BaseInventoryStorageComponent, outComponents);
		foreach (Managed componentRef : outComponents)
		{
			if (BaseInventoryStorageComponent.Cast(componentRef).Contains(child)) return false;
		}
		*/

		// Check if entity is attached to any of the parents slots
		SlotManagerComponent slotManager = EL_Component<SlotManagerComponent>.Find(parent);
		if (slotManager)
		{
			array<EntitySlotInfo> outSlots();
			slotManager.GetSlotInfos(outSlots);
			foreach (EntitySlotInfo slot : outSlots)
			{
				if (slot.GetAttachedEntity() == child) return false;
			}
		}

		// Check attachment slot components
		parent.FindComponents(AttachmentSlotComponent, outComponents);
		foreach (Managed componentRef : outComponents)
		{
			if (AttachmentSlotComponent.Cast(componentRef).GetAttachedEntity() == child) return false;
		}

		// Check magazines in case parent might be a weapon
		parent.FindComponents(BaseMuzzleComponent, outComponents);
		foreach (Managed componentRef : outComponents)
		{
			if (BaseMuzzleComponent.Cast(componentRef).GetMagazine() == child) return false;
		}

		return true;
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
