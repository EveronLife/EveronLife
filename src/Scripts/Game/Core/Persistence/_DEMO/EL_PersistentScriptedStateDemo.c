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
		
		AsyncApi(repository);
	}
	
	protected void SyncApi(EL_DbEntityRepository<EL_PersistentExampleState> repository)
	{
		//Insert states
		repository.AddOrUpdate(new EL_PersistentExampleState(1337, 42.42, true, "Hello World"));
		
		//Get all states
		array<ref EL_PersistentExampleState> states = repository.FindAll();
		PrintFormat("EL_PersistentScriptedStateDemo::FindViaSyncApi() -> states: %1", states);
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