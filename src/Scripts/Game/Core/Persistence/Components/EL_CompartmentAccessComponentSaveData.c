[EL_ComponentSaveDataType(EL_CompartmentAccessComponentSaveDataClass, CompartmentAccessComponent), BaseContainerProps()]
class EL_CompartmentAccessComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

[EL_DbName(EL_CompartmentAccessComponentSaveData, "CompartmentAccess")]
class EL_CompartmentAccessComponentSaveData : EL_ComponentSaveData
{
	string m_sEntity;
	int m_iSlotIdx;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		CompartmentAccessComponent compartment = CompartmentAccessComponent.Cast(worldEntityComponent);
		BaseCompartmentSlot comparmentSlot = compartment.GetCompartment();
		if (comparmentSlot)
		{
			IEntity owner = comparmentSlot.GetOwner();
			BaseCompartmentManagerComponent compartmentManager = EL_Component<BaseCompartmentManagerComponent>.Find(owner);
			while (compartmentManager)
			{
				IEntity parent = owner.GetParent();
				BaseCompartmentManagerComponent parentManager = EL_Component<BaseCompartmentManagerComponent>.Find(parent);
				if (!parentManager)
					break;

				owner = parent;
				compartmentManager = parentManager;
			}

			if (compartmentManager)
			{
				array<BaseCompartmentSlot> outCompartments();
				compartmentManager.GetCompartments(outCompartments);
				foreach (int idx, BaseCompartmentSlot slot : outCompartments)
				{
					if (slot == comparmentSlot)
					{
						m_sEntity = EL_PersistenceComponent.GetPersistentId(owner);
						m_iSlotIdx = idx;
					}
				}
			}
		}
		else
		{
			m_iSlotIdx = -1;
		}

		if (!m_sEntity || m_iSlotIdx == -1) return EL_EReadResult.DEFAULT;
		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(notnull GenericComponent worldEntityComponent, notnull EL_ComponentSaveDataClass attributes)
	{
		CompartmentAccessComponent compartment = CompartmentAccessComponent.Cast(worldEntityComponent);
		IEntity compartmentHolder = EL_PersistenceManager.GetInstance().FindEntityByPersistentId(m_sEntity);
		BaseCompartmentManagerComponent compartmentManager = EL_Component<BaseCompartmentManagerComponent>.Find(compartmentHolder);
		if (compartmentManager)
		{
			array<BaseCompartmentSlot> outCompartments();
			compartmentManager.GetCompartments(outCompartments);
			if (m_iSlotIdx < outCompartments.Count())
			{
				compartment.MoveInVehicle(compartmentHolder, outCompartments.Get(m_iSlotIdx));
				return EL_EApplyResult.OK;
			}
		}

		// No longer able to enter compartment so find free position on the ground to spawn instead
		IEntity owner = compartment.GetOwner();
		vector currentPos = owner.GetOrigin();
		if (SCR_WorldTools.FindEmptyTerrainPosition(currentPos, currentPos, 50, cylinderHeight: 1000))
			EL_Utils.Teleport(owner, currentPos);

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_CompartmentAccessComponentSaveData otherData = EL_CompartmentAccessComponentSaveData.Cast(other);
		return true;
	}
};
