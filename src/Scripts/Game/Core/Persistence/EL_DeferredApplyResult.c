class EL_DeferredApplyResult
{
	protected static ref map<ref EL_EntitySaveData, ref EL_PendingIdentifierHolder> s_mPendingIdentifiers =
		new map<ref EL_EntitySaveData, ref EL_PendingIdentifierHolder>;

	protected static ref map<EL_ComponentSaveData, ref EL_PendingComponentIdentifierHolder> s_mPendingComponentIdentifiers =
		new map<EL_ComponentSaveData, ref EL_PendingComponentIdentifierHolder>;

	protected static ref array<EL_EntitySaveData> s_aCheckQueue;

	//------------------------------------------------------------------------------------------------
	static ScriptInvoker GetOnApplied(notnull EL_EntitySaveData saveData)
	{
		EL_PendingIdentifierHolder data = s_mPendingIdentifiers.Get(saveData);
		if (!data)
			return null;

		return data.m_pOnAppliedEvent;
	}

	//------------------------------------------------------------------------------------------------
	static void AddPending(notnull EL_EntitySaveData saveData, string awaitIdentifier)
	{
		EL_PendingIdentifierHolder data = s_mPendingIdentifiers.Get(saveData);
		if (!data)
		{
			data = new EL_PendingIdentifierHolder({awaitIdentifier});
			s_mPendingIdentifiers.Set(saveData, data);
			return;
		}

		data.m_aIdentifiers.Insert(awaitIdentifier);
	}

	//------------------------------------------------------------------------------------------------
	static void AddPending(notnull EL_ComponentSaveData componentSaveData, string awaitIdentifier)
	{
		EL_PendingComponentIdentifierHolder data = s_mPendingComponentIdentifiers.Get(componentSaveData);
		if (!data)
		{
			data = EL_PendingComponentIdentifierHolder({awaitIdentifier});
			s_mPendingComponentIdentifiers.Set(componentSaveData, data);
			return;
		}

		data.m_aIdentifiers.Insert(awaitIdentifier);
	}

	//------------------------------------------------------------------------------------------------
	static void SetFinished(notnull EL_EntitySaveData saveData, string awaitIdentifier)
	{
		EL_PendingIdentifierHolder data = s_mPendingIdentifiers.Get(saveData);
		if (!data)
			return;

		data.m_aIdentifiers.RemoveItem(awaitIdentifier);

		if (data.m_aIdentifiers.IsEmpty())
			QueueComplectionCheck(saveData);
	}

	//------------------------------------------------------------------------------------------------
	static void SetFinished(notnull EL_ComponentSaveData componentSaveData, string awaitIdentifier)
	{
		EL_PendingComponentIdentifierHolder data = s_mPendingComponentIdentifiers.Get(componentSaveData);
		if (!data)
			return;

		data.m_aIdentifiers.RemoveItem(awaitIdentifier);
		if (!data.m_aIdentifiers.IsEmpty())
			return;

		if (data.m_pSaveData)
		{
			QueueComplectionCheck(data.m_pSaveData);
		}
		else
		{
			// Remove info if pending + finished without ask for event or SetEntitySaveData invoke.
			s_mPendingComponentIdentifiers.Remove(componentSaveData);
		}
	}

	//------------------------------------------------------------------------------------------------
	static bool SetEntitySaveData(notnull EL_ComponentSaveData componentSaveData, notnull EL_EntitySaveData entitySaveData)
	{
		EL_PendingComponentIdentifierHolder componentData = s_mPendingComponentIdentifiers.Get(componentSaveData);
		if (componentData)
		{
			componentData.m_pSaveData = entitySaveData;

			EL_PendingIdentifierHolder data;
			if (s_mPendingIdentifiers)
				data = s_mPendingIdentifiers.Get(entitySaveData);

			if (!data)
			{
				data = new EL_PendingIdentifierHolder(components: {componentSaveData});
				s_mPendingIdentifiers.Set(entitySaveData, data);
			}
			else
			{
				data.m_aComponents.Insert(componentSaveData);
			}

			return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static void QueueComplectionCheck(EL_EntitySaveData saveData)
	{
		if (!s_aCheckQueue)
			s_aCheckQueue = {};

		s_aCheckQueue.Insert(saveData);

		ScriptCallQueue callQueue = GetGame().GetCallqueue();
		if (callQueue.GetRemainingTime(CheckForCompletion) == -1)
			callQueue.Call(CheckForCompletion);
	}

	//------------------------------------------------------------------------------------------------
	protected static void CheckForCompletion()
	{
		foreach (EL_EntitySaveData saveData : s_aCheckQueue)
		{
			EL_PendingIdentifierHolder data = s_mPendingIdentifiers.Get(saveData);

			if (!data.m_aIdentifiers.IsEmpty())
				continue;

			bool completed = true;
			foreach (EL_ComponentSaveData componentSaveData : data.m_aComponents)
			{
				EL_PendingComponentIdentifierHolder componentData = s_mPendingComponentIdentifiers.Get(componentSaveData);
				if (!componentData.m_aIdentifiers.IsEmpty())
					completed = false;
			}
			if (!completed)
				continue;

			if (data.m_pOnAppliedEvent)
				data.m_pOnAppliedEvent.Invoke(saveData);

			// Free save data again after apply has finished
			foreach (EL_ComponentSaveData componentSaveData : data.m_aComponents)
			{
				s_mPendingComponentIdentifiers.Remove(componentSaveData);
			}
			s_mPendingIdentifiers.Remove(saveData);
		}

		s_aCheckQueue.Clear();
	}
};

class EL_PendingIdentifierHolder
{
	ref array<string> m_aIdentifiers;
	ref array<EL_ComponentSaveData> m_aComponents;
	ref ScriptInvoker<EL_EntitySaveData> m_pOnAppliedEvent;

	void EL_PendingIdentifierHolder(array<string> identifiers = null, array<EL_ComponentSaveData> components = null)
	{
		m_aIdentifiers = identifiers;
		if (!m_aIdentifiers)
			m_aIdentifiers = {};

		m_aComponents = components;
		if (!m_aComponents)
			m_aComponents = {};

		m_pOnAppliedEvent = new ScriptInvoker();
	}
};

class EL_PendingComponentIdentifierHolder
{
	EL_EntitySaveData m_pSaveData;
	ref array<string> m_aIdentifiers;

	void EL_PendingComponentIdentifierHolder(array<string> identifiers = null)
	{
		m_aIdentifiers = identifiers;
		if (!m_aIdentifiers)
			m_aIdentifiers = {};
	}
};
