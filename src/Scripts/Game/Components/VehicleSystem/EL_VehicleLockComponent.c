[ComponentEditorProps(category: "EveronLife/Feature/Vehicles", description: "Locks a vehicle to players with a matching key.")]
class EL_VehicleLockComponentClass : SCR_BaseLockComponentClass
{
}

class EL_VehicleLockComponent : SCR_BaseLockComponent
{
	[Attribute("")]
	protected string m_sDebugIdentifier;

	[RplProp()]
	protected string m_sVehicleIdentifier;

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		if (m_sDebugIdentifier != ""){
			m_sVehicleIdentifier = m_sDebugIdentifier;
		}
			
		
		SetLocked(true);
	}

	//------------------------------------------------------------------------------------------------
	void ToggleLocked()
	{
		SetLocked(!m_bIsLocked);
	}

	//------------------------------------------------------------------------------------------------
	bool IsVehicleLocked()
	{
		return m_bIsLocked;
	}

	//------------------------------------------------------------------------------------------------
	override bool IsLocked(IEntity user, BaseCompartmentSlot compartmentSlot)
	{
		if (!user)
			return true;

		if (compartmentSlot && m_pVehicleSpawnProtection && m_pVehicleSpawnProtection.IsProtected(user, compartmentSlot))
			return false;

		return m_bIsLocked;
	}

	//------------------------------------------------------------------------------------------------
	bool IsValidKey(IEntity key)
	{
		if(key == null)
			return false;
		
		EL_VehicleKeyComponent keyComp = EL_VehicleKeyComponent.Cast(key.FindComponent(EL_VehicleKeyComponent));
		
		if(keyComp == null)
			return false;

		return keyComp.GetVehicleIdentifier() == m_sVehicleIdentifier;
	}

	//------------------------------------------------------------------------------------------------
	bool UserHasValidKey(IEntity user)
	{
		CharacterControllerComponent characterController = CharacterControllerComponent.Cast(user.FindComponent(CharacterControllerComponent));
		if (characterController && IsValidKey(characterController.GetAttachedGadgetAtLeftHandSlot()))
			return true;

		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;

		array<IEntity> inventoryItems();
		inventoryManager.GetItems(inventoryItems);

		foreach (IEntity item : inventoryItems)
		{
			if (IsValidKey(item))
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	static bool CheckCanInteract(IEntity owner, IEntity user)
	{
		if (!owner || !user)
			return true;

		Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(owner, true));
		if (!vehicle)
			return true;

		EL_VehicleLockComponent vehicleLock = EL_VehicleLockComponent.Cast(vehicle.FindComponent(EL_VehicleLockComponent));
		if (!vehicleLock)
			return true;

		return !vehicleLock.IsVehicleLocked();
	}

	//------------------------------------------------------------------------------------------------
	override LocalizedString GetCannotPerformReason(IEntity user)
	{
		if (m_bIsLocked)
			return "Locked";

		return super.GetCannotPerformReason(user);
	}
}
