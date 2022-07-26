class EL_PersistentExampleState : EL_PersistentSciptedState
{
	int m_IntegerValue;
	float m_FloatValue;
	bool m_BooleanValue;
	string m_StringValue;
	
	void EL_PersistentExampleState(int intVal = 0, float floatVal = 0.0, bool boolVal = false, string stringVal = "")
	{
		m_IntegerValue = intVal;
		m_FloatValue = floatVal;
		m_BooleanValue = boolVal;
		m_StringValue = stringVal;
	}
}

class EL_PersistentScriptedStateDemo
{
	void Run()
	{
		EL_DbEntityRepository<EL_PersistentExampleState> repository = EL_DbEntityHelper<EL_PersistentExampleState>.GetRepository();
		Print(repository);

		repository.AddOrUpdateAsync(new EL_PersistentExampleState(1337, 42.9999, true, "Hello World"));
		repository.AddOrUpdateAsync(new EL_PersistentExampleState(2222, 22.2223451, true, "State 2"));
		repository.AddOrUpdateAsync(new EL_PersistentExampleState(3333, 33.3334543, false, "State 3"));
		repository.AddOrUpdateAsync(new EL_PersistentExampleState(4444, 44.4434535, true, "State 4"));
		repository.AddOrUpdateAsync(new EL_PersistentExampleState(12345, 3.456, false, "State 5"));
		repository.AddOrUpdateAsync(new EL_PersistentExampleState(12345, 1.234, false, "State 6"));
		repository.AddOrUpdateAsync(new EL_PersistentExampleState(12345, 2.345, false, "State 7"));
		
		AsyncApi(repository);
		
		thread SyncApi(repository);
	}
	
	protected void SyncApi(EL_DbEntityRepository<EL_PersistentExampleState> repository)
	{
		//Get all states
		array<ref EL_PersistentExampleState> allNoOrder = repository.FindAll();
		PrintFormat("EL_PersistentScriptedStateDemo::SyncApi() -> allNoOrder: %1", allNoOrder);
		
		//Find by id
		EL_PersistentExampleState findByIdResult = repository.Find(allNoOrder.Get(0).GetId());
		PrintFormat("EL_PersistentScriptedStateDemo::SyncApi() -> findByIdResult: %1(%2)", findByIdResult, findByIdResult.m_StringValue);
		
		//Find first by bool field
		EL_PersistentExampleState findFirstBoolFalse = repository.FindFirst(EL_DbFind.Field("m_BooleanValue").Equals(false));
		PrintFormat("EL_PersistentScriptedStateDemo::SyncApi() -> findFirstBoolFalse: %1(%2)", findFirstBoolFalse, findFirstBoolFalse.m_StringValue);
		
		//Find by complex conditions and order results
		array<ref EL_PersistentExampleState> complexAndOrdered = repository.FindAll(
			EL_DbFind.And({
				EL_DbFind.Field("m_StringValue").Contains("State"),
				EL_DbFind.Field("m_FloatValue").GreaterThanOrEqual(33.0)
			}), 
			orderBy: {{"m_StringValue", "ASC"}});
		
		PrintFormat("EL_PersistentScriptedStateDemo::SyncApi() -> complexAndOrdered:");
		foreach(EL_PersistentExampleState state : complexAndOrdered)
		{
			PrintFormat("%1(%2, %3, %4, '%5')", state.Type().ToString(), state.m_IntegerValue, state.m_FloatValue, state.m_BooleanValue, state.m_StringValue);
		}
	}

	protected void AsyncApi(EL_DbEntityRepository<EL_PersistentExampleState> repository)
	{
		//Find async via callback class
		repository.FindAllAsync(callback: new EL_ExampleScriptedStateFindCallback());
		
		//Find async via callback method
		repository.FindAllAsync(callback: EL_DbFindCallback<EL_PersistentExampleState>.FromMethod(this, "OnDataLoaded"));
	}
	
	protected void OnDataLoaded(EL_DbOperationStatusCode resultCode, array<ref EL_PersistentExampleState> resultData)
	{
		PrintFormat("EL_PersistentScriptedStateDemo::OnDataLoaded(%1, %2)", typename.EnumToString(EL_DbOperationStatusCode, resultCode), resultData);
	}
}

class EL_ExampleScriptedStateFindCallback : EL_DbFindCallback<EL_PersistentExampleState>
{
	override void OnSuccess(array<ref EL_PersistentExampleState> resultData)
	{
		PrintFormat("EL_ExampleScriptedStateFindCallback::OnSuccess(%1)", resultData);
	}
	
	override void OnFailure(EL_DbOperationStatusCode resultCode)
	{
		PrintFormat("EL_ExampleScriptedStateFindCallback::OnFailure(%1)", typename.EnumToString(EL_DbOperationStatusCode, resultCode));
	}
}