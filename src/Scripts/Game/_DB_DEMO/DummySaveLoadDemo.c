class EL_DummyDbEntity : EL_DbEntity
{
	int m_IntegerValue;
	float m_FloatValue;
	bool m_BooleanValue;
	string m_StringValue;
}

class CustomDbDummyCallback : EL_DbFindCallback<EL_DummyDbEntity>
{
	override void OnSuccess(array<ref EL_DummyDbEntity> resultData)
	{
		PrintFormat("CustomDbDummyCallback::OnSuccess(%1)", resultData);
	}
	
	override void OnFailure(EL_DbOperationStatusCode resultCode)
	{
		PrintFormat("CustomDbDummyCallback::OnFailure(%1)", typename.EnumToString(EL_DbOperationStatusCode, resultCode));
	}
}

class EL_DBDummySaveLoadDemo
{
	void LoadFromDbVariantWithFunction()
	{
		EL_DbEntityRepository<EL_DummyDbEntity> repository = EL_DbEntityHelper<EL_DummyDbEntity>.GetRepository();
		Print(repository);
		
		repository.FindAllAsync(callback: new CustomDbDummyCallback());
		
		repository.FindAllAsync(callback: EL_DbFindCallback<EL_DummyDbEntity>.FromMethod(this, "OnDataLoaded"));
	}
	
	void OnDataLoaded(EL_DbOperationStatusCode resultCode, array<ref EL_DummyDbEntity> resultData)
	{
		PrintFormat("EL_DBDummySaveLoadDemo::OnDataLoaded(%1, %2)", typename.EnumToString(EL_DbOperationStatusCode, resultCode), resultData);
	}
}
