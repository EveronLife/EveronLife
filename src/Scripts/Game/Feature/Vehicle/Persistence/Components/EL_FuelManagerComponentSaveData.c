[EL_ComponentSaveDataType(EL_FuelManagerComponentSaveData, FuelManagerComponent, "FuelManager"), BaseContainerProps()]
class EL_FuelManagerComponentSaveData : EL_ComponentSaveDataBase
{
	ref array<ref EL_PersistentFuelNode> m_aFuelNodes;

	//------------------------------------------------------------------------------------------------
	override bool ReadFrom(notnull GenericComponent worldEntityComponent)
	{
		m_aFuelNodes = new array<ref EL_PersistentFuelNode>();

		array<BaseFuelNode> outNodes();
		FuelManagerComponent.Cast(worldEntityComponent).GetFuelNodesList(outNodes);

		foreach (BaseFuelNode node : outNodes)
		{
			SCR_FuelNode fuelNode = SCR_FuelNode.Cast(node);
			if (!fuelNode)
			{
				Debug.Error(string.Format("'%1' contains non persistable fuel node type '%2'. Inherit from SCR_FuelNode instead. Ignored.", worldEntityComponent, node.Type()));
				continue;
			}

			EL_PersistentFuelNode persistentFuelNode();
			persistentFuelNode.m_iTankId = fuelNode.GetFuelTankID();
			persistentFuelNode.m_fFuel = fuelNode.GetFuel();
			m_aFuelNodes.Insert(persistentFuelNode);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool ApplyTo(notnull GenericComponent worldEntityComponent)
	{
		array<BaseFuelNode> outNodes();
		FuelManagerComponent.Cast(worldEntityComponent).GetFuelNodesList(outNodes);

		bool tryIdxAcces = outNodes.Count() >= m_aFuelNodes.Count();

		foreach (int idx, EL_PersistentFuelNode persistentFuelNode : m_aFuelNodes)
		{
			SCR_FuelNode fuelNode;

			// Assume same ordering as on save and see if that matches
			if (tryIdxAcces)
			{
				SCR_FuelNode idxNode = SCR_FuelNode.Cast(outNodes.Get(idx));

				if (idxNode.GetFuelTankID() == persistentFuelNode.m_iTankId) fuelNode = idxNode;
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
				Debug.Error(string.Format("'%1' unable to find fuel tank id '%2'. Ignored.", worldEntityComponent, persistentFuelNode.m_iTankId));
				continue;
			}

			fuelNode.SetFuel(persistentFuelNode.m_fFuel);
		}

		return true;
	}
}

class EL_PersistentFuelNode
{
	int m_iTankId;
	float m_fFuel;
}
