class EL_DbFind
{
	//------------------------------------------------------------------------------------------------
	static EL_DbFindAnd And(notnull array<ref EL_DbFindCondition> conditions)
	{
		return EL_DbFindAnd.Create(conditions);
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindOr Or(notnull array<ref EL_DbFindCondition> conditions)
	{
		return EL_DbFindOr.Create(conditions);
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindFieldCollectionHandlingBuilder Field(string fieldPath)
	{
		return EL_DbFindFieldCollectionHandlingBuilder.Create(fieldPath);
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindFieldPrimitiveValueConditonBuilder Id()
	{
		// Return with only primitive field builder options (implicit cast)
		return EL_DbFindFieldCollectionHandlingBuilder.Create(EL_DbEntity.FIELD_ID);
	}
}

class EL_DbFindCondition
{
	protected static ref array<ref EL_DbFindCondition> ALLOC_BUFFER;

	//------------------------------------------------------------------------------------------------
	sealed void Debug()
	{
		PrintFormat("%1:\n%2", this, GetDebugString());
	}

	//------------------------------------------------------------------------------------------------
	string GetDebugString();

	//------------------------------------------------------------------------------------------------
	//! Collects id field comparision values and returns true if there are no other fields that conditions need to be applied to
	static bool CollectConditionIds(EL_DbFindCondition condition, out set<string> ids)
	{
		EL_DbFindFieldStringMultiple stringMultipleCondition = EL_DbFindFieldStringMultiple.Cast(condition);
		if (stringMultipleCondition)
		{
			if (stringMultipleCondition.m_sFieldPath != EL_DbEntity.FIELD_ID ||
				stringMultipleCondition.m_eComparisonOperator != EL_EDbFindOperator.EQUAL) return false;

			foreach (string id : stringMultipleCondition.m_aComparisonValues)
			{
				ids.Insert(id);
			}

			return true;
		}

		EL_DbFindConditionWithChildren conditionWithChildren = EL_DbFindConditionWithChildren.Cast(condition);
		if (conditionWithChildren)
		{
			foreach (EL_DbFindCondition childCondition : conditionWithChildren.m_Conditions)
			{
				if (!CollectConditionIds(childCondition, ids)) return false;
			}

			return true;
		}

		// TODO: Can be optimized to check if id field is part of AND, and there are no other toplevel ORs, so we can know that only specific ids need to be loaded and then filters applied.
		return false;
	}
}

class EL_DbFindConditionWithChildren : EL_DbFindCondition
{
	ref array<ref EL_DbFindCondition> m_Conditions;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		if (!m_Conditions) return "NULL";

		string dbg = "(";

		foreach (int nCondtion, EL_DbFindCondition condition : m_Conditions)
		{
			dbg += "\n";

			array<string> conditionLines();
			condition.GetDebugString().Split("\n", conditionLines, true);

			foreach (int nLine, string line : conditionLines)
			{
				dbg += string.Format("    %1", line);

				if (nLine != conditionLines.Count() -1) dbg += "\n";
			}

			if (nCondtion == m_Conditions.Count() - 1)
			{
				 dbg += "\n";
			}
			else
			{
				dbg += ",";
			}
		}

		return dbg + ")";
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindConditionWithChildren(notnull array<ref EL_DbFindCondition> conditions)
	{
		m_Conditions = conditions;
	}
}

class EL_DbFindAnd : EL_DbFindConditionWithChildren
{
	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		return "And" + super.GetDebugString();
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindAnd Create(notnull array<ref EL_DbFindCondition> conditions)
	{
		EL_DbFindAnd inst = new EL_DbFindAnd(conditions);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {NULL};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
}

class EL_DbFindOr : EL_DbFindConditionWithChildren
{
	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		return "Or" + super.GetDebugString();
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindOr Create(notnull array<ref EL_DbFindCondition> conditions)
	{
		EL_DbFindOr inst = new EL_DbFindOr(conditions);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {NULL};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
}

class EL_DbFindFieldCondition : EL_DbFindCondition
{
	string m_sFieldPath;
}

class EL_DbFindCheckFieldNull : EL_DbFindFieldCondition
{
	bool m_bShouldBeNull;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		if (m_bShouldBeNull) return string.Format("CheckNull(fieldPath:'%1', shouldBeNull:true)", m_sFieldPath);

		return string.Format("CheckNull(fieldPath:'%1', shouldBeNull:false)", m_sFieldPath);
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindCheckFieldNull Create(string fieldPath, bool shouldBeNull)
	{
		auto inst = new EL_DbFindCheckFieldNull(fieldPath, shouldBeNull);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {NULL};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindCheckFieldNull(string fieldPath, bool shouldBeNull)
	{
		m_sFieldPath = fieldPath;
		m_bShouldBeNull = shouldBeNull;
	}
}

class EL_DbFindCheckFieldEmpty : EL_DbFindFieldCondition
{
	bool m_ShouldBeEmpty;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		if (m_ShouldBeEmpty) return string.Format("CheckEmpty(fieldPath:'%1', shouldBeEmpty:true)", m_sFieldPath);

		return string.Format("CheckEmpty(fieldPath:'%1', shouldBeEmpty:false)", m_sFieldPath);
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindCheckFieldEmpty Create(string fieldPath, bool shouldBeEmpty)
	{
		auto inst = new EL_DbFindCheckFieldEmpty(fieldPath, shouldBeEmpty);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {NULL};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindCheckFieldEmpty(string fieldPath, bool shouldBeEmpty)
	{
		m_sFieldPath = fieldPath;
		m_ShouldBeEmpty = shouldBeEmpty;
	}
}

enum EL_EDbFindOperator
{
	EQUAL,
	NOT_EQUAL,
	LESS_THAN,
	LESS_THAN_OR_EQUAL,
	GREATER_THAN,
	GREATER_THAN_OR_EQUAL,
	CONTAINS,
	NOT_CONTAINS
}

class EL_DbFindCompareFieldValues<Class ValueType> : EL_DbFindFieldCondition
{
	private static ref array<ref EL_DbFindCompareFieldValues<ValueType>> ALLOC_BUFFER_TVALUES;

	EL_EDbFindOperator m_eComparisonOperator;
	ref array<ValueType> m_aComparisonValues;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		string valuesString = "{";
		foreach (int idx, ValueType value : m_aComparisonValues)
		{
			if (idx != 0) valuesString += ",";

			if (idx > 10)
			{
				valuesString += "...";
				break;
			}

			typename valueType = ValueType;

			if (valueType.IsInherited(bool))
			{
				if (value)
				{
					valuesString += "true";
				}
				else
				{
					valuesString += "false";
				}
			}
			else
			{
				valuesString += string.Format("%1", value);
			}
		}
		valuesString += "}";

		return string.Format("Compare(fieldPath:'%1', operator:%2, values:%3)", m_sFieldPath, typename.EnumToString(EL_EDbFindOperator, m_eComparisonOperator), valuesString);
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindCompareFieldValues<ValueType> Create(string fieldPath, EL_EDbFindOperator comparisonOperator, notnull array<ValueType> comparisonValues)
	{
		auto inst = new EL_DbFindCompareFieldValues<ValueType>(fieldPath, comparisonOperator, comparisonValues);
		if (!ALLOC_BUFFER_TVALUES) ALLOC_BUFFER_TVALUES = {NULL};
		ALLOC_BUFFER_TVALUES.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindCompareFieldValues(string fieldPath, EL_EDbFindOperator comparisonOperator, notnull array<ValueType> comparisonValues)
	{
		m_sFieldPath = fieldPath;
		m_eComparisonOperator = comparisonOperator;
		m_aComparisonValues = comparisonValues;
	}
}

typedef EL_DbFindCompareFieldValues<int> EL_DbFindFieldIntMultiple;
typedef EL_DbFindCompareFieldValues<float> EL_DbFindFieldFloatMultiple;
typedef EL_DbFindCompareFieldValues<bool> EL_DbFindFieldBoolMultiple;
typedef EL_DbFindCompareFieldValues<string> EL_DbFindFieldStringMultiple;
typedef EL_DbFindCompareFieldValues<vector> EL_DbFindFieldVectorMultiple;

class EL_DbValues<Class T>
{
	protected static ref array<ref array<T>> ALLOC_BUFFER;

	//------------------------------------------------------------------------------------------------
	static array<T> From(notnull array<T> values)
	{
		auto data = new array<T>();
		data.Reserve(values.Count());

		foreach (T value : values)
		{
			data.Insert(value);
		}

		if (!ALLOC_BUFFER) ALLOC_BUFFER = {NULL};
		ALLOC_BUFFER.Set(0, data);
		return data;
	}
}

typedef EL_DbValues<int> EL_DbValuesInt;
typedef EL_DbValues<float> EL_DbValuesFloat;
typedef EL_DbValues<bool> EL_DbValuesBool;
typedef EL_DbValues<string> EL_DbValuesString;
typedef EL_DbValues<vector> EL_DbValuesVector;

class EL_DbFindFieldAnnotations
{
	const string SEPERATOR = ".";
	const string ANY = ":any";
	const string ALL = ":all";
	const string COUNT = ":count";
	const string KEYS = ":keys";
	const string VALUES = ":values";
}

class EL_DbFindFieldConditionBuilder
{
	string m_sFieldPath;
	bool m_bInverted;

	//------------------------------------------------------------------------------------------------
	protected void _AppendIfNotPresent(string pathValue)
	{
		if (m_sFieldPath.EndsWith(pathValue)) return;

		m_sFieldPath += pathValue;
	}

	//------------------------------------------------------------------------------------------------
	protected array<string> _ConvertTypenameArray(array<typename> values)
	{
		array<string> valuesString();
		valuesString.Reserve(values.Count());
		foreach (typename type : values)
		{
			valuesString.Insert(type.ToString());
		}

		return valuesString;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindFieldConditionBuilder(string fieldPath, bool inverted = false)
	{
		m_sFieldPath = fieldPath;
		m_bInverted = inverted;
	}
}

class EL_DbFindFieldNumericValueConditonBuilder : EL_DbFindFieldConditionBuilder
{
	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(int comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(float comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition EqualsAnyOf(notnull array<int> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition EqualsAnyOf(notnull array<float> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition LessThan(int comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition LessThan(float comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition LessThanOrEqual(int comparisonValue) // TODO: Rename to LessThanOrEquals as soon as https://feedback.bistudio.com/T166821 is fixed
	{
		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN, {comparisonValue});

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition LessThanOrEqual(float comparisonValue) // see LessThanOrEqual(int)
	{
		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN, {comparisonValue});

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition GreaterThan(int comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition GreaterThan(float comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition GreaterThanOrEqual(int comparisonValue) // see LessThanOrEqual(int)
	{
		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN, {comparisonValue});

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition GreaterThanOrEqual(float comparisonValue) // see LessThanOrEqual(int)
	{
		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN, {comparisonValue});

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Between(int lowerBound, int upperBound)
	{
		if (m_bInverted)
		{
			return EL_DbFindOr.Create({
				 EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN, {lowerBound}),
				 EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN, {upperBound})
			});
		}

		return EL_DbFindAnd.Create({
			 EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN_OR_EQUAL, {lowerBound}),
			 EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN_OR_EQUAL, {upperBound})
		});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Between(float lowerBound, float upperBound)
	{
		if (m_bInverted)
		{
			return EL_DbFindOr.Create({
				 EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN, {lowerBound}),
				 EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN, {upperBound})
			});
		}

		return EL_DbFindAnd.Create({
			 EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN_OR_EQUAL, {lowerBound}),
			 EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN_OR_EQUAL, {upperBound})
		});
	}
}

class EL_DbFindFieldPrimitiveValueConditonBuilder : EL_DbFindFieldNumericValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(bool comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldBoolMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EL_DbFindFieldBoolMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(string comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(vector comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition EqualsAnyOf(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition EqualsAnyOf(notnull array<vector> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition GreaterThan(vector comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});

		return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition GreaterThanOrEqual(vector comparisonValue) // see LessThanOrEqual(int)
	{
		if (m_bInverted) return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.LESS_THAN, {comparisonValue});

		return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Contains(string comparisonValue)
	{
		if (m_bInverted) return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, {comparisonValue});

		return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAnyOf(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldNumericValueConditonBuilder Length()
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.COUNT);
		return this;
	}
}

class EL_DbFindFieldAllValueConditonBuilder : EL_DbFindFieldPrimitiveValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Null()
	{
		return EL_DbFindCheckFieldNull.Create(m_sFieldPath, !m_bInverted);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Empty()
	{
		return EL_DbFindCheckFieldEmpty.Create(m_sFieldPath, !m_bInverted);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(typename comparisonValue)
	{
		return Equals(comparisonValue.ToString());
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition EqualsAnyOf(notnull array<typename> comparisonValues)
	{
		return EqualsAnyOf(_ConvertTypenameArray(comparisonValues));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(notnull array<int> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(notnull array<float> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(notnull array<bool> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldBoolMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldBoolMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(notnull array<vector> comparisonValues)
	{
		if (m_bInverted) return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Equals(notnull array<typename> comparisonValues)
	{
		return Equals(_ConvertTypenameArray(comparisonValues));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Contains(int comparisonValue)
	{
		return ContainsAnyOf(EL_DbValues<int>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Contains(float comparisonValue)
	{
		return ContainsAnyOf(EL_DbValues<float>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Contains(bool comparisonValue)
	{
		return ContainsAnyOf(EL_DbValues<bool>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Contains(vector comparisonValue)
	{
		return ContainsAnyOf(EL_DbValues<vector>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition Contains(typename comparisonValue)
	{
		return ContainsAnyOf(EL_DbValues<string>.From({comparisonValue.ToString()}));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAnyOf(notnull array<int> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAnyOf(notnull array<float> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAnyOf(notnull array<bool> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EL_DbFindFieldBoolMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldBoolMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAnyOf(notnull array<vector> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAnyOf(notnull array<typename> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ANY);

		return ContainsAnyOf(_ConvertTypenameArray(comparisonValues));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAllOf(notnull array<int> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldIntMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAllOf(notnull array<float> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldFloatMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAllOf(notnull array<bool> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EL_DbFindFieldBoolMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldBoolMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAllOf(notnull array<string> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldStringMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAllOf(notnull array<vector> comparisonValues)
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EL_DbFindFieldVectorMultiple.Create(m_sFieldPath, EL_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindCondition ContainsAllOf(notnull array<typename> comparisonValues)
	{
		return ContainsAllOf(_ConvertTypenameArray(comparisonValues));
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldNumericValueConditonBuilder Count()
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.COUNT);
		return this;
	}
}

class EL_DbFindFieldMainConditionBuilder : EL_DbFindFieldAllValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldAllValueConditonBuilder Not()
	{
		m_bInverted = true;
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldCollectionHandlingBuilder Field(string fieldPath)
	{
		m_sFieldPath += EL_DbFindFieldAnnotations.SEPERATOR + fieldPath;
		return EL_DbFindFieldCollectionHandlingBuilder.Cast(this);
	}
}

class EL_DbFindFieldCollectionHandlingBuilder : EL_DbFindFieldMainConditionBuilder
{
	protected static ref array<ref EL_DbFindFieldCollectionHandlingBuilder>> ALLOC_BUFFER;

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldMainConditionBuilder Any()
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ANY);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldMainConditionBuilder All()
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.ALL);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldCollectionHandlingBuilder Keys()
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.KEYS);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldCollectionHandlingBuilder Values()
	{
		_AppendIfNotPresent(EL_DbFindFieldAnnotations.VALUES);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldMainConditionBuilder At(int index)
	{
		return Field(index.ToString());
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldMainConditionBuilder FirstOf(typename complexType)
	{
		return Any().Field(complexType.ToString());
	}

	//------------------------------------------------------------------------------------------------
	EL_DbFindFieldMainConditionBuilder AllOf(typename complexType)
	{
		return All().Field(complexType.ToString());
	}

	//------------------------------------------------------------------------------------------------
	static EL_DbFindFieldCollectionHandlingBuilder Create(string fieldPath)
	{
		auto inst = new EL_DbFindFieldCollectionHandlingBuilder(fieldPath);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {NULL};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
}
