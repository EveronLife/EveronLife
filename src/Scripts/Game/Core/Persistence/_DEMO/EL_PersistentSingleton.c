[EL_PersistentSciptedStateStruct(EL_PersistentMySingleton, EL_MySingleton)]
class EL_PersistentMySingleton : EL_PersistentSciptedState
{
	int intVal;
	string stringVal;

	override bool ReadFrom(Class scriptedState)
	{
		EL_MySingleton.Cast(scriptedState).WriteToPersistence(this);
		
		return true;
	}
	
	override bool ApplyTo(Class scriptedState)
	{
		EL_MySingleton.Cast(scriptedState).ReadFromPersistence(this);
		
		return true;
	}
}

class EL_MySingleton
{
	protected static ref EL_MySingleton s_pInstance;
	
	protected int intVal;
	protected string stringVal;
	protected bool otherVar;
	
	static EL_MySingleton GetInstance()
	{
		if(!s_pInstance)
		{
			s_pInstance = EL_PersistentSciptedStateSpawner<EL_MySingleton>.GetSingleton();
		}

		return s_pInstance;
	}

	void WriteToPersistence(EL_PersistentMySingleton persistenceData)
	{
		persistenceData.intVal = intVal;
		persistenceData.stringVal = stringVal;
	}
	
	void ReadFromPersistence(EL_PersistentMySingleton persistenceData)
	{
		intVal = persistenceData.intVal;
		stringVal = persistenceData.stringVal;
	}
	
	void EL_MySingleton()
	{
		EL_PersistenceManager.GetInstance().StartTacking(this);
	}

	void ~EL_MySingleton()
	{
		EL_PersistenceManager.GetInstance().StopTacking(this);
	}
}
