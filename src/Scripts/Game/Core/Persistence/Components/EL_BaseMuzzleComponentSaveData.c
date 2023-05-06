[BaseContainerProps()]
class EL_BaseMuzzleComponentSaveDataClass : EL_ComponentSaveDataClass
{
};

class EL_BaseMuzzleComponentSaveData : EL_ComponentSaveData
{
	EMuzzleType m_eMuzzleType;
	ref array<bool> m_aChamberStatus;

	//------------------------------------------------------------------------------------------------
	override EL_EReadResult ReadFrom(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseMuzzleComponent muzzle = BaseMuzzleComponent.Cast(component);
		int barrelsCount = muzzle.GetBarrelsCount();

		m_eMuzzleType = muzzle.GetMuzzleType();
		m_aChamberStatus = {};
		m_aChamberStatus.Reserve(barrelsCount);

		bool isDefaultChambered = IsDefaultChambered(owner, component, attributes);
		bool isDefault = true;
		for (int nBarrel = 0; nBarrel < barrelsCount; nBarrel++)
		{
			bool isChambered = muzzle.IsBarrelChambered(nBarrel);
			if (isChambered != isDefaultChambered)
				isDefault = false;

			m_aChamberStatus.Insert(isChambered);
		}

		if (isDefault)
			return EL_EReadResult.DEFAULT;

		return EL_EReadResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsDefaultChambered(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes);

	//------------------------------------------------------------------------------------------------
	override EL_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseMuzzleComponent muzzle = BaseMuzzleComponent.Cast(component);

		foreach (int idx, bool chambered : m_aChamberStatus)
		{
			if (!chambered)
				muzzle.ClearChamber(idx);
		}

		return EL_EApplyResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool IsFor(IEntity owner, GenericComponent component, EL_ComponentSaveDataClass attributes)
	{
		BaseMuzzleComponent muzzle = BaseMuzzleComponent.Cast(component);
		return muzzle.GetMuzzleType() == m_eMuzzleType && muzzle.GetBarrelsCount() == m_aChamberStatus.Count();
	}

	//------------------------------------------------------------------------------------------------
	override bool Equals(notnull EL_ComponentSaveData other)
	{
		EL_BaseMuzzleComponentSaveData otherData = EL_BaseMuzzleComponentSaveData.Cast(other);

		if (m_eMuzzleType != otherData.m_eMuzzleType ||
			m_aChamberStatus.Count() != otherData.m_aChamberStatus.Count())
			return false;

		foreach (int idx, bool chambered : m_aChamberStatus)
		{
			if (chambered != m_aChamberStatus.Get(idx))
				return false;
		}

		return true;
	}
};
