class EL_DbFindCondition
{
	static EL_DbFindCondition And(notnull array<ref EL_DbFindCondition> conditions)
	{
		return new EL_DbFindAnd(conditions);
	}
	
	static EL_DbFindCondition Or(notnull array<ref EL_DbFindCondition> conditions)
	{
		return new EL_DbFindOr(conditions);
	}
	
	static EL_DbFindCondition Field(string fieldName, EL_DbNumericFieldOperator comparisonOperator, int comparisonValue)
	{
		return new EL_DbFindCompareFieldInt(fieldName, comparisonOperator, comparisonValue);
	}
	
	static EL_DbFindCondition Field(string fieldName, EL_DbNumericFieldOperator comparisonOperator, float comparisonValue)
	{
		return new EL_DbFindCompareFieldFloat(fieldName, comparisonOperator, comparisonValue);
	}
	
	static EL_DbFindCondition Field(string fieldName, bool comparisonValue)
	{
		return new EL_DbFindCompareFieldBool(fieldName, comparisonValue);
	}
	
	static EL_DbFindCondition Field(string fieldName, EL_DbStringFieldOperator comparisonOperator, string comparisonValue)
	{
		return new EL_DbFindCompareFieldString(fieldName, comparisonOperator, comparisonValue);
	}
	
	// TODO FieldContains function to array<int>, array<string> etc?
}

class EL_DbFindConditionWithChildren : EL_DbFindCondition
{
	ref array<ref EL_DbFindCondition> m_Conditions;
	
	void EL_DbFindConditionWithChildren(notnull array<ref EL_DbFindCondition> conditions)
	{
		m_Conditions = conditions;
	}
}

class EL_DbFindAnd : EL_DbFindConditionWithChildren
{
}

class EL_DbFindOr : EL_DbFindConditionWithChildren
{
}

enum EL_DbNumericFieldOperator
{
	EQUAL,
	NOT_EQUAL,
	LESS_THAN,
	LESS_THAN_OR_EQUAL,
	GREATER_THAN,
	GREATER_THAN_OR_EQUAL
}

class EL_DbFindCompareFieldInt : EL_DbFindCondition
{
	protected string m_FieldName;
	protected EL_DbNumericFieldOperator m_ComparisonOperator;
	protected int m_ComparisonValue;
	
	void EL_DbFindCompareFieldInt(string fieldName, EL_DbNumericFieldOperator comparisonOperator, int comparisonValue)
	{
		m_FieldName = fieldName;
		m_ComparisonOperator = comparisonOperator;
		m_ComparisonValue = comparisonValue;
	}
}

class EL_DbFindCompareFieldFloat : EL_DbFindCondition
{
	protected string m_FieldName;
	protected EL_DbNumericFieldOperator m_ComparisonOperator;
	protected float m_ComparisonValue;
	
	void EL_DbFindCompareFieldFloat(string fieldName, EL_DbNumericFieldOperator comparisonOperator, float comparisonValue)
	{
		m_FieldName = fieldName;
		m_ComparisonOperator = comparisonOperator;
		m_ComparisonValue = comparisonValue;
	}
}

class EL_DbFindCompareFieldBool : EL_DbFindCondition
{
	protected string m_FieldName;
	protected bool m_ComparisonValue;
	
	void EL_DbFindCompareFieldBool(string fieldName, bool comparisonValue)
	{
		m_FieldName = fieldName;
		m_ComparisonValue = comparisonValue;
	}
}

enum EL_DbStringFieldOperator
{
	EQUAL,
	NOT_EQUAL,
	CONTAINS,
	NOT_CONTAINS
}

class EL_DbFindCompareFieldString : EL_DbFindCondition
{
	protected string m_FieldName;
	protected EL_DbNumericFieldOperator m_ComparisonOperator;
	protected string m_ComparisonValue;
	
	void EL_DbFindCompareFieldString(string fieldName, EL_DbStringFieldOperator comparisonOperator, string comparisonValue)
	{
		m_FieldName = fieldName;
		m_ComparisonOperator = comparisonOperator;
		m_ComparisonValue = comparisonValue;
	}
}
