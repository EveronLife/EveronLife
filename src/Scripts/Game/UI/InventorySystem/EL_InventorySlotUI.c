modded class SCR_InventorySlotUI
{
	private EL_InventoryQuantityComponent m_EL_QuantityComponent;

	override void UpdateReferencedComponent(InventoryItemComponent pComponent)
	{
		super.UpdateReferencedComponent(pComponent);

		if (!pComponent)
		{
			return;
		}

		m_EL_QuantityComponent = EL_InventoryQuantityComponent.Cast(pComponent.GetOwner().FindComponent(EL_InventoryQuantityComponent));
		if (m_EL_QuantityComponent)
		{
			SetStackNumber(m_EL_QuantityComponent.GetQuantity());
		}
	}

	EL_InventoryQuantityComponent EL_GetInventoryQuantityComponent()
	{
		return m_EL_QuantityComponent;
	}
	
	override void UpdateStackNumber()
	{
		super.UpdateStackNumber();

		if (!m_EL_QuantityComponent || !m_wStackNumber)
		{
			return;
		}

		array<ref Tuple2<int, string>> numerals();
		numerals.Insert(new Tuple2<int, string>(0, ""));
		numerals.Insert(new Tuple2<int, string>(1000, "K"));
		numerals.Insert(new Tuple2<int, string>(1000000, "M"));
		numerals.Insert(new Tuple2<int, string>(1000000000, "B"));

		for (int i = 0; i < numerals.Count(); i++)
		{
			if (m_iStackNumber > numerals[i].param1)
			{
				continue;
			}

			auto numeral = numerals[i - 1];
			int divisor = numeral.param1;
			int quantity = m_iStackNumber;
			if (divisor != 0)
			{
				quantity /= divisor;
			}

			m_wStackNumber.SetText(quantity.ToString() + numeral.param2);
			break;
		}
	}
};
