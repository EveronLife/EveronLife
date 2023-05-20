[EL_ComponentSaveDataType(FuelManagerComponent), BaseContainerProps()]
class EL_FuelManagerComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

class EL_FuelManagerComponentSaveData : EL_ComponentSaveData
{
	ref array<ref EL_PersistentFuelNode> m_aFuelNodes;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		m_aFuelNodes = {};

		array<BaseFuelNode> outNodes();
		FuelManagerComponent.Cast(component).GetFuelNodesList(outNodes);

		foreach (BaseFuelNode node : outNodes)
		{
			SCR_FuelNode fuelNode = SCR_FuelNode.Cast(node);
			if (!fuelNode)
			{
				Debug.Error(string.Format("'%1' contains non persistable fuel node type '%2'. Inherit from SCR_FuelNode instead. Ignored.", component, node.Type()));
				continue;
			}

			EL_PersistentFuelNode persistentFuelNode();
			persistentFuelNode.m_iTankId = fuelNode.GetFuelTankID();
			persistentFuelNode.m_fFuel = fuelNode.GetFuel();

			if (attributes.m_bTrimDefaults)
			{
				if (persistentFuelNode.m_fFuel >= fuelNode.GetMaxFuel())
					continue;

				// Remove relfection getter when https://feedback.bistudio.com/T171947 is resolved in 0.9.9
				float initalFuelState;
				if (EL_ReflectionUtilsT<float>.Get(fuelNode, "m_fInitialFuelTankState", initalFuelState) &&
					float.AlmostEqual(persistentFuelNode.m_fFuel, initalFuelState)) continue;
			}

			m_aFuelNodes.Insert(persistentFuelNode);
		}

		if (m_aFuelNodes.IsEmpty())
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		array<BaseFuelNode> outNodes();
		FuelManagerComponent.Cast(component).GetFuelNodesList(outNodes);

		bool tryIdxAcces = outNodes.Count() >= m_aFuelNodes.Count();

		foreach (int idx, EL_PersistentFuelNode persistentFuelNode : m_aFuelNodes)
		{
			SCR_FuelNode fuelNode;

			// Assume same ordering as on save and see if that matches
			if (tryIdxAcces)
			{
				SCR_FuelNode idxNode = SCR_FuelNode.Cast(outNodes.Get(idx));

				if (idxNode.GetFuelTankID() == persistentFuelNode.m_iTankId)
					fuelNode = idxNode;
			}

			// Iterate all fuel nodes to hopefully find the right tank id
			if (!fuelNode)
			{
				foreach (BaseFuelNode findNode : outNodes)
				{
					SCR_FuelNode findFuelNode = SCR_FuelNode.Cast(findNode);
					if (findFuelNode && findFuelNode.GetFuelTankID() == persistentFuelNode.m_iTankId)
					{
						fuelNode = findFuelNode;
						break;
					}
				}
			}

			if (!fuelNode)
			{
				Debug.Error(string.Format("'%1' unable to find fuel tank id '%2'. Ignored.", component, persistentFuelNode.m_iTankId));
				continue;
			}

			fuelNode.SetFuel(persistentFuelNode.m_fFuel);
		}

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_FuelManagerComponentSaveData otherData = EL_FuelManagerComponentSaveData.Cast(other);

		if (m_aFuelNodes.Count() != otherData.m_aFuelNodes.Count())
			return false;

		foreach (int idx, EL_PersistentFuelNode fuelNode : m_aFuelNodes)
		{
			// Try same index first as they are likely to be the correct ones.
			if (fuelNode.Equals(otherData.m_aFuelNodes.Get(idx)))
				continue;

			bool found;
			foreach (int compareIdx, EL_PersistentFuelNode otherFuelNode : otherData.m_aFuelNodes)
			{
				if (compareIdx == idx)
					continue; // Already tried in idx direct compare

				if (fuelNode.Equals(otherFuelNode))
				{
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}

		return true;
	}
};

class EL_PersistentFuelNode
{
	int m_iTankId;
	float m_fFuel;

	//------------------------------------------------------------------------------------------------
	bool Equals(notnull EL_PersistentFuelNode other)
	{
		return m_iTankId == other.m_iTankId && float.AlmostEqual(m_fFuel, other.m_fFuel);
	}
};
