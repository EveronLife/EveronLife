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
		
		thread SyncApi(repository);
		
		//AsyncApi(repository);
	}
	
	protected void SyncApi(EL_DbEntityRepository<EL_PersistentExampleState> repository)
	{
		//Insert states
		EL_PersistentExampleState state1(1337, 42.9999, true, "Hello World");
		repository.AddOrUpdate(state1);
		repository.AddOrUpdate(new EL_PersistentExampleState(2222, 22.2223451, true, "State 2"));
		repository.AddOrUpdate(new EL_PersistentExampleState(3333, 33.3334543, false, "State 3"));
		repository.AddOrUpdate(new EL_PersistentExampleState(4444, 44.4434535, true, "State 4"));
		repository.AddOrUpdate(new EL_PersistentExampleState(12345, 3.456, false, "State 5"));
		repository.AddOrUpdate(new EL_PersistentExampleState(12345, 1.234, false, "State 6"));
		repository.AddOrUpdate(new EL_PersistentExampleState(12345, 2.345, false, "State 7"));
		
		//Get all states
		array<ref EL_PersistentExampleState> allNoOrder = repository.FindAll();
		PrintFormat("EL_PersistentScriptedStateDemo::SyncApi() -> allNoOrder: %1", allNoOrder);
		
		//Find by id
		EL_PersistentExampleState findByIdResult = repository.Find(state1.GetId());
		PrintFormat("EL_PersistentScriptedStateDemo::SyncApi() -> findByIdResult: %1", findByIdResult);
		Print(findByIdResult.m_StringValue);
		
		//Find first by bool field
		EL_PersistentExampleState findFirstBoolFalse = repository.FindFirst(EL_DbFindCondition.Field("m_BooleanValue", false));
		PrintFormat("EL_PersistentScriptedStateDemo::SyncApi() -> findFirstBoolFalse: %1", findFirstBoolFalse);
		Print(findFirstBoolFalse.m_StringValue);
		
		array<ref EL_PersistentExampleState> allOrderedByNameDesc = repository.FindAll(orderBy: {{"m_StringValue", "DESC"}});
		foreach(EL_PersistentExampleState state : allOrderedByNameDesc)
		{
			Print(state.m_StringValue);
		}
	}

	protected void AsyncApi(EL_DbEntityRepository<EL_PersistentExampleState> repository)
	{
		//Uses insert from sync api call
		
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