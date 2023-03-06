enum EL_DbFindFieldPathSegmentFlags
{
	NUMBER = 1,
	TYPENAME = 2,
	ANY = 4,
	ALL = 8,
	KEYS = 16,
	VALUES = 32,
	COUNT = 64
}

class EL_DbFindFieldPathSegment
{
	string m_sFieldName;
	int m_iFlags;

	//------------------------------------------------------------------------------------------------
	void EL_DbFindFieldPathSegment(string fieldName, int flags)
	{
		m_sFieldName = fieldName;
		m_iFlags = flags;
	}
}

enum EL_DbFindFieldCollectionType
{
	NONE,
	ARRAY,
	SET,
	MAP
}

class EL_DbFindFieldTypeInfo
{
	protected static ref map<string, ref EL_DbFindFieldTypeInfo> s_mTypeCache;

	int m_iVariableindex;
	typename m_tType;
	typename m_tCollectionKeyType;
	typename m_tCollectionValueType;
	EL_DbFindFieldCollectionType m_eCollectionType;

	//------------------------------------------------------------------------------------------------
	static EL_DbFindFieldTypeInfo Get(Class instance, string fieldName)
	{
		if (!s_mTypeCache)
		{
			s_mTypeCache = new map<string, ref EL_DbFindFieldTypeInfo>();
		}

		string typeCacheKey = string.Format("%1::%2", instance.Type().ToString(), fieldName);

		EL_DbFindFieldTypeInfo info = s_mTypeCache.Get(typeCacheKey);

		if (!info)
		{
			typename type = instance.Type();

			for (int vIdx = 0; vIdx < type.GetVariableCount(); vIdx++)
			{
				string variableName = type.GetVariableName(vIdx);

				if (!variableName) break;

				if (variableName == fieldName)
				{
					info = new EL_DbFindFieldTypeInfo(vIdx, type.GetVariableType(vIdx));
					break;
				}
			}

			if (info)
			{
				s_mTypeCache.Set(typeCacheKey, info);
			}
		}

		return info;
	}

	//------------------------------------------------------------------------------------------------
	void EL_DbFindFieldTypeInfo(int variableIndex, typename type)
	{
		m_iVariableindex = variableIndex;
		m_tType = type;

		if (m_tType.IsInherited(array) || m_tType.IsInherited(set))
		{
			if (m_tType.IsInherited(array))
			{
				m_eCollectionType = EL_DbFindFieldCollectionType.ARRAY;
			}
			else
			{
				m_eCollectionType = EL_DbFindFieldCollectionType.SET;
			}

			string typeString = m_tType.ToString();
			typeString.Replace("@", "");

			int templateStart = typeString.IndexOf("<") + 1;
			string collectionValueTypeString = typeString.Substring(templateStart, typeString.Length() - templateStart - 1);
			m_tCollectionValueType = collectionValueTypeString.ToType();
		}
		else if (m_tType.IsInherited(map))
		{
			m_eCollectionType = EL_DbFindFieldCollectionType.MAP;

			string typeString = m_tType.ToString();
			typeString.Replace("@", "");

			int keyTypeStart = typeString.IndexOf("<") + 1;
			int valueTypeStart = typeString.IndexOfFrom(keyTypeStart, ",") + 1;

			string collectionKeyTypeString = typeString.Substring(keyTypeStart, valueTypeStart - keyTypeStart - 1);
			m_tCollectionKeyType = collectionKeyTypeString.ToType();

			string collectionValueTypeString = typeString.Substring(valueTypeStart, typeString.Length() - valueTypeStart - 1);
			m_tCollectionValueType = collectionValueTypeString.ToType();
		}
	}
}

class EL_DbFindConditionEvaluator
{
	//------------------------------------------------------------------------------------------------
	static array<ref EL_DbEntity> GetFiltered(notnull array<ref EL_DbEntity> entities, notnull EL_DbFindCondition condition)
	{
		array<ref EL_DbEntity> conditionMatched();

		foreach (EL_DbEntity entity : entities)
		{
			if (EL_DbFindConditionEvaluator.Evaluate(entity, condition))
			{
				conditionMatched.Insert(entity);
			}
		}

		return conditionMatched;
	}

	//------------------------------------------------------------------------------------------------
	static bool Evaluate(notnull EL_DbEntity entity, notnull EL_DbFindCondition condition)
	{
		switch(condition.Type())
		{
			case EL_DbFindAnd:
			{
				foreach (EL_DbFindCondition checkCondition : EL_DbFindAnd.Cast(condition).m_Conditions)
				{
					if (!Evaluate(entity, checkCondition)) return false;
				}

				return true;
			}

			case EL_DbFindOr:
			{
				foreach (EL_DbFindCondition checkCondition : EL_DbFindOr.Cast(condition).m_Conditions)
				{
					if (Evaluate(entity, checkCondition)) return true;
				}

				return false;
			}

			default:
			{
				EL_DbFindFieldCondition fieldCondition = EL_DbFindFieldCondition.Cast(condition);

				if (!fieldCondition) return false;

				return EvaluateField(entity, fieldCondition, ParseSegments(fieldCondition), 0);
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static array<ref EL_DbFindFieldPathSegment> ParseSegments(EL_DbFindFieldCondition fieldCondition)
	{
		array<ref EL_DbFindFieldPathSegment> resultSegments();

		array<string> segments();
		fieldCondition.m_sFieldPath.Split(EL_DbFindFieldAnnotations.SEPERATOR, segments, true);

		resultSegments.Reserve(segments.Count());

		foreach (string segment : segments)
		{
			int flags;

			while (true)
			{
				if (segment.Replace(EL_DbFindFieldAnnotations.ANY, "") > 0)
				{
					flags |= EL_DbFindFieldPathSegmentFlags.ANY;
					continue;
				}

				if (segment.Replace(EL_DbFindFieldAnnotations.ALL, "") > 0)
				{
					flags |= EL_DbFindFieldPathSegmentFlags.ALL;
					continue;
				}

				if (segment.Replace(EL_DbFindFieldAnnotations.KEYS, "") > 0)
				{
					flags |= EL_DbFindFieldPathSegmentFlags.KEYS;
					continue;
				}

				if (segment.Replace(EL_DbFindFieldAnnotations.VALUES, "") > 0)
				{
					flags |= EL_DbFindFieldPathSegmentFlags.VALUES;
					continue;
				}

				if (segment.Replace(EL_DbFindFieldAnnotations.COUNT, "") > 0)
				{
					flags |= EL_DbFindFieldPathSegmentFlags.COUNT;
					continue;
				}

				break;
			}

			int asIntValue = segment.ToInt();
			if (asIntValue == 0 && segment == "0")
			{
				flags |= EL_DbFindFieldPathSegmentFlags.NUMBER;
			}
			else if (segment.ToType())
			{
				flags |= EL_DbFindFieldPathSegmentFlags.TYPENAME;
			}

			resultSegments.Insert(new EL_DbFindFieldPathSegment(segment, flags));
		}

		return resultSegments;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool EvaluateField(Class instance, EL_DbFindFieldCondition fieldCondition, array<ref EL_DbFindFieldPathSegment> pathSegments, int currentSegmentIndex)
	{
		if (currentSegmentIndex >= pathSegments.Count())
		{
			Debug.Error(string.Format("Failed to evaluate invalid rule '%1' on '%2'.", fieldCondition.m_sFieldPath, instance));
			return false;
		}

		EL_DbFindFieldPathSegment currentSegment = pathSegments.Get(currentSegmentIndex);
		EL_DbFindFieldTypeInfo typeInfo = EL_DbFindFieldTypeInfo.Get(instance, currentSegment.m_sFieldName);

		// Expand complex/collection type as this is not yet the final path segment
		if (currentSegmentIndex < pathSegments.Count() - 1)
		{
			if (typeInfo.m_tType.IsInherited(Class))
			{
				Class complexFieldValue;
				if (!instance.Type().GetVariableValue(instance, typeInfo.m_iVariableindex, complexFieldValue))
				{
					Debug.Error(string.Format("Failed to read field '%1' of type '%2' on '%3'.", currentSegment.m_sFieldName, typeInfo.m_tType, instance));
					return false;
				}

				// Expand collections
				if (typeInfo.m_eCollectionType != EL_DbFindFieldCollectionType.NONE)
				{
					int collectionCount;
					GetGame().GetScriptModule().Call(complexFieldValue, "Count", false, collectionCount);

					int nextSegmentIndex = currentSegmentIndex + 1;

					EL_DbFindFieldPathSegment nextSegment = pathSegments.Get(nextSegmentIndex);

					// Handle collection<Class>.typename access operator
					typename filterType = typename.Empty;
					if (nextSegment.m_iFlags & EL_DbFindFieldPathSegmentFlags.TYPENAME)
					{
						filterType = nextSegment.m_sFieldName.ToType();
						nextSegmentIndex++; //Skip source.filterType. to condition after the filter
					}

					// Handle collection<...>.N access operator
					int collectionStartIndex = 0;
					bool indexAccessorMode = false;
					if (nextSegment.m_iFlags & EL_DbFindFieldPathSegmentFlags.NUMBER)
					{
						collectionStartIndex = nextSegment.m_sFieldName.ToInt();

						if (collectionStartIndex >= collectionCount)
						{
							Debug.Error(string.Format("Tried to access ilegal collection index '%1' of type '%2' on '%3'. Collection only contained '%4' items.", collectionStartIndex, typeInfo.m_tType, instance, collectionCount));
							return false;
						}

						indexAccessorMode = true;
						nextSegmentIndex++; // Continue on the segment after the index information
					}

					for (int nValue = collectionStartIndex; nValue < collectionCount; nValue++)
					{
						Class collectionValueItem;

						string getFnc = "Get";

						// Access n-th map element by key(default) or value
						if (typeInfo.m_eCollectionType == EL_DbFindFieldCollectionType.MAP)
						{
							if (currentSegment.m_iFlags & EL_DbFindFieldPathSegmentFlags.VALUES) //Only if Values() was explicitly set
							{
								getFnc = "GetElement";
							}
							else
							{
								getFnc = "GetKey";
							}
						}

						if (!GetGame().GetScriptModule().Call(complexFieldValue, getFnc, false, collectionValueItem, nValue) || !collectionValueItem)
						{
							Debug.Error(string.Format("Failed to get collection value at index '%1' on collection '%2' on '%3'.", nValue, complexFieldValue, instance));
							return false;
						}

						if (filterType && !collectionValueItem.IsInherited(filterType)) continue;

						bool evaluationResult = EvaluateField(collectionValueItem, fieldCondition, pathSegments, nextSegmentIndex);

						// A specific index of the collection was checked, just directly return that result what ever it is.
						if (indexAccessorMode) return evaluationResult;

						if ((currentSegment.m_iFlags & EL_DbFindFieldPathSegmentFlags.ALL))
						{
							// All must be true, but at least one was false, so total result is false
							if (!evaluationResult) return false;

							// All elements were iterated and none of them returned false, so the overall result is true
							if (nValue == collectionCount - 1) return true;
						}

						//Any(default) must be true, and we got one match, so we can abort and return true
						if (evaluationResult) return true;
					}

					//None of the typename filters matched or none of the any-conditions returned true
					return false;
				}

				// Expand complex type
				return EvaluateField(complexFieldValue, fieldCondition, pathSegments, currentSegmentIndex + 1);
			}
			else
			{
				Debug.Error(string.Format("Reading field '%1' of a primtive type '%2' on '%3' is not possible.", currentSegment.m_sFieldName, typeInfo.m_tType, instance));
				return false;
			}
		}

		// Apply condition to result field value
		switch(fieldCondition.Type())
		{
			case EL_DbFindCheckFieldNull:
			{
				if (typeInfo.m_tType.IsInherited(Class))
				{
					Class fieldValue;
					if (!instance.Type().GetVariableValue(instance, typeInfo.m_iVariableindex, fieldValue)) return false;
					return fieldValue == null;
				}

				Debug.Error(string.Format("Can no null check field '%1' of a primtive type '%2' on '%3' is not possible.", currentSegment.m_sFieldName, typeInfo.m_tType, instance));
				return false;
			}

			case EL_DbFindCheckFieldEmpty:
			{
				switch(typeInfo.m_tType)
				{
					case int: return EL_DbFindFielEmptyChecker<int>.Evaluate(instance, EL_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, typeInfo);
					case float: return EL_DbFindFielEmptyChecker<float>.Evaluate(instance, EL_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, typeInfo);
					case bool: return EL_DbFindFielEmptyChecker<bool>.Evaluate(instance, EL_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, typeInfo);
					case string: return EL_DbFindFielEmptyChecker<string>.Evaluate(instance, EL_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, typeInfo);
					case vector: return EL_DbFindFielEmptyChecker<vector>.Evaluate(instance, EL_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, typeInfo);
				}

				return EL_DbFindFielEmptyChecker<Class>.Evaluate(instance, EL_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, typeInfo);
			}

			case EL_DbFindFieldIntMultiple:
			{
				return EL_DbFindFieldValueTypedEvaluator<int>.Evaluate(instance, EL_DbFindFieldIntMultiple.Cast(fieldCondition), currentSegment, typeInfo);
			}

			case EL_DbFindFieldFloatMultiple:
			{
				return EL_DbFindFieldValueTypedEvaluator<float>.Evaluate(instance, EL_DbFindFieldFloatMultiple.Cast(fieldCondition), currentSegment, typeInfo);
			}

			case EL_DbFindFieldBoolMultiple:
			{
				return EL_DbFindFieldValueTypedEvaluator<bool>.Evaluate(instance, EL_DbFindFieldBoolMultiple.Cast(fieldCondition), currentSegment, typeInfo);
			}

			case EL_DbFindFieldStringMultiple:
			{
				return EL_DbFindFieldValueTypedEvaluator<string>.Evaluate(instance, EL_DbFindFieldStringMultiple.Cast(fieldCondition), currentSegment, typeInfo);
			}

			case EL_DbFindFieldVectorMultiple:
			{
				return EL_DbFindFieldValueTypedEvaluator<vector>.Evaluate(instance, EL_DbFindFieldVectorMultiple.Cast(fieldCondition), currentSegment, typeInfo);
			}

			default:
			{
				Debug.Error(string.Format("Unknown condition type '%1'.", fieldCondition.Type().ToString()));
				return false;
			}
		}

		// Fall through
		return false;
	}
}

class EL_DbFindFielEmptyChecker<Class TValueType>
{
	//------------------------------------------------------------------------------------------------
	static bool Evaluate(Class instance, EL_DbFindCheckFieldEmpty valueCondition, EL_DbFindFieldPathSegment currentSegment, EL_DbFindFieldTypeInfo fieldInfo)
	{
		TValueType fieldValue;
		if (!instance.Type().GetVariableValue(instance, fieldInfo.m_iVariableindex, fieldValue)) return false;

		return IsEmptyOrDefault(fieldValue);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(int value)
	{
		return value == 0;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(float value)
	{
		return float.AlmostEqual(value, 0.0);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(bool value)
	{
		return value == false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(string value)
	{
		value.Replace(" ", "");
		return value.IsEmpty();
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(vector value)
	{
		return float.AlmostEqual(vector.Distance(value, "0 0 0"), 0.0);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(Class value)
	{
		if (value.Type().IsInherited(array) || value.Type().IsInherited(set) || value.Type().IsInherited(map))
		{
			int collectionCount;
			if (!GetGame().GetScriptModule().Call(value, "Count", false, collectionCount)) return false;
			return collectionCount == 0;
		}

		return value == null;
	}
}

class EL_DbFindFieldValueTypedEvaluator<Class TValueType>
{
	//------------------------------------------------------------------------------------------------
	static bool Evaluate(Class instance, EL_DbFindCompareFieldValues<TValueType> valueCondition, EL_DbFindFieldPathSegment currentSegment, EL_DbFindFieldTypeInfo fieldInfo)
	{
		typename valueType = TValueType;

		if (valueCondition.m_aComparisonValues.IsEmpty())
		{
			Debug.Error(string.Format("Can not compare field '%1' on '%2' with empty condition.", currentSegment.m_sFieldName, instance));
			return false;
		}

		// Handle collection comparison
		if (fieldInfo.m_eCollectionType != EL_DbFindFieldCollectionType.NONE)
		{
			Class collectionHolder;
			if (!instance.Type().GetVariableValue(instance, fieldInfo.m_iVariableindex, collectionHolder)) return false;

			int collectionCount;
			if (!GetGame().GetScriptModule().Call(collectionHolder, "Count", false, collectionCount)) return false;

			// Handle count of collection comparison
			if (currentSegment.m_iFlags & EL_DbFindFieldPathSegmentFlags.COUNT)
			{
				return CompareCollectionCount(collectionCount, valueCondition.m_eComparisonOperator, valueCondition.m_aComparisonValues);
			}

			bool exactOrderedMatch = (valueCondition.m_eComparisonOperator == EL_EDbFindOperator.EQUAL) &&
				((currentSegment.m_iFlags & (EL_DbFindFieldPathSegmentFlags.ANY | EL_DbFindFieldPathSegmentFlags.ALL)) == 0);

			// If the count missmatches on full match it can not be equal
			if (exactOrderedMatch && (collectionCount != valueCondition.m_aComparisonValues.Count())) return false;

			for (int nItem = 0; nItem < collectionCount; nItem++)
			{
				TValueType fieldValue;

				string getFnc = "Get";

				if (fieldInfo.m_eCollectionType == EL_DbFindFieldCollectionType.MAP)
				{
					if (currentSegment.m_iFlags & EL_DbFindFieldPathSegmentFlags.VALUES) //Only if Values() was explicitly set
					{
						getFnc = "GetElement";
					}
					else
					{
						getFnc = "GetKey";
					}
				}

				if (!GetGame().GetScriptModule().Call(collectionHolder, getFnc, false, fieldValue, nItem)) return false;

				array<TValueType> compareValues = valueCondition.m_aComparisonValues;
				if (exactOrderedMatch) compareValues = {valueCondition.m_aComparisonValues.Get(nItem)};

				bool comparisonMatches = Compare(fieldValue, valueCondition.m_eComparisonOperator, compareValues);

				if (exactOrderedMatch || (currentSegment.m_iFlags & EL_DbFindFieldPathSegmentFlags.ALL))
				{
					// All must match the operator comparison, if one of them failes the final result is false
					if (!comparisonMatches) return false;

					// All including the last item in the collection matched the comparison, so we return true
					if (nItem == collectionCount - 1) return true;
				}
				else
				{
					// Any of the item matched, so we can return true early
					if (comparisonMatches) return true;

					// None, including the last item, matched the condition so return false
					if (nItem == collectionCount - 1) return false;
				}
			}

			// Fall through
			return false;
		}

		// Compare primitive value
		TValueType fieldValue;
		if (!instance.Type().GetVariableValue(instance, fieldInfo.m_iVariableindex, fieldValue)) return false;
		return Compare(fieldValue, valueCondition.m_eComparisonOperator, valueCondition.m_aComparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool CompareCollectionCount(int collectionCount, EL_EDbFindOperator operator, Class comparisonValues)
	{
		array<int> strongTypedComparisonValues = array<int>.Cast(comparisonValues);
		if (!strongTypedComparisonValues) return false;

		return Compare(collectionCount, operator, strongTypedComparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(int fieldValue, EL_EDbFindOperator operator, array<int> comparisonValues)
	{
		switch(operator)
		{
			case EL_EDbFindOperator.CONTAINS:
			case EL_EDbFindOperator.EQUAL:
			{
				return comparisonValues.Contains(fieldValue);
			}

			case EL_EDbFindOperator.NOT_CONTAINS:
			case EL_EDbFindOperator.NOT_EQUAL:
			{
				return !comparisonValues.Contains(fieldValue);
			}

			case EL_EDbFindOperator.LESS_THAN:
			{
				foreach (int compare : comparisonValues)
				{
					if (fieldValue < compare) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.LESS_THAN_OR_EQUAL:
			{
				foreach (int compare : comparisonValues)
				{
					if (fieldValue <= compare) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.GREATER_THAN:
			{
				foreach (int compare : comparisonValues)
				{
					if (fieldValue > compare) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.GREATER_THAN_OR_EQUAL:
			{
				foreach (int compare : comparisonValues)
				{
					if (fieldValue >= compare) return true;
				}

				return false;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(float fieldValue, EL_EDbFindOperator operator, array<float> comparisonValues)
	{
		switch(operator)
		{
			case EL_EDbFindOperator.CONTAINS:
			case EL_EDbFindOperator.EQUAL:
			{
				foreach (float compare : comparisonValues)
				{
					if (float.AlmostEqual(fieldValue, compare)) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.NOT_CONTAINS:
			case EL_EDbFindOperator.NOT_EQUAL:
			{
				foreach (float compare : comparisonValues)
				{
					if (float.AlmostEqual(fieldValue, compare)) return false;
				}

				return true;
			}

			case EL_EDbFindOperator.LESS_THAN:
			{
				foreach (float compare : comparisonValues)
				{
					if (fieldValue < compare) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.LESS_THAN_OR_EQUAL:
			{
				foreach (float compare : comparisonValues)
				{
					if ((fieldValue < compare) || float.AlmostEqual(fieldValue, compare)) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.GREATER_THAN:
			{
				foreach (float compare : comparisonValues)
				{
					if (fieldValue > compare) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.GREATER_THAN_OR_EQUAL:
			{
				foreach (float compare : comparisonValues)
				{
					if ((fieldValue > compare) || float.AlmostEqual(fieldValue, compare)) return true;
				}

				return false;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(bool fieldValue, EL_EDbFindOperator operator, array<bool> comparisonValues)
	{
		switch(operator)
		{
			case EL_EDbFindOperator.CONTAINS:
			case EL_EDbFindOperator.EQUAL:
			{
				return comparisonValues.Contains(fieldValue);
			}

			case EL_EDbFindOperator.NOT_CONTAINS:
			case EL_EDbFindOperator.NOT_EQUAL:
			{
				return !comparisonValues.Contains(fieldValue);
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(string fieldValue, EL_EDbFindOperator operator, array<string> comparisonValues)
	{
		switch(operator)
		{
			case EL_EDbFindOperator.EQUAL:
			{
				return comparisonValues.Contains(fieldValue);
			}

			case EL_EDbFindOperator.NOT_EQUAL:
			{
				return !comparisonValues.Contains(fieldValue);
			}

			case EL_EDbFindOperator.CONTAINS:
			{
				foreach (string compare : comparisonValues)
				{
					if (fieldValue.Contains(compare)) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.NOT_CONTAINS:
			{
				foreach (string compare : comparisonValues)
				{
					if (fieldValue.Contains(compare)) return false;
				}

				return true;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(vector fieldValue, EL_EDbFindOperator operator, array<vector> comparisonValues)
	{
		switch(operator)
		{
			case EL_EDbFindOperator.CONTAINS:
			case EL_EDbFindOperator.EQUAL:
			{
				foreach (vector compare : comparisonValues)
				{
					if (float.AlmostEqual(vector.Distance(fieldValue, compare), 0)) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.NOT_CONTAINS:
			case EL_EDbFindOperator.NOT_EQUAL:
			{
				foreach (vector compare : comparisonValues)
				{
					if (float.AlmostEqual(vector.Distance(fieldValue, compare), 0)) return false;
				}

				return true;
			}

			case EL_EDbFindOperator.LESS_THAN:
			{
				foreach (vector compare : comparisonValues)
				{
					if ((fieldValue[0] < compare[0]) &&
						(fieldValue[1] < compare[1]) &&
						(fieldValue[2] < compare[2])) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.LESS_THAN_OR_EQUAL:
			{
				foreach (vector compare : comparisonValues)
				{
					if (((fieldValue[0] < compare[0]) || float.AlmostEqual(fieldValue[0], compare[0])) &&
						((fieldValue[1] < compare[1]) || float.AlmostEqual(fieldValue[1], compare[1])) &&
						((fieldValue[2] < compare[2]) || float.AlmostEqual(fieldValue[2], compare[2]))) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.GREATER_THAN:
			{
				foreach (vector compare : comparisonValues)
				{
					if ((fieldValue[0] > compare[0]) &&
						(fieldValue[1] > compare[1]) &&
						(fieldValue[2] > compare[2])) return true;
				}

				return false;
			}

			case EL_EDbFindOperator.GREATER_THAN_OR_EQUAL:
			{
				foreach (vector compare : comparisonValues)
				{
					if (((fieldValue[0] > compare[0]) || float.AlmostEqual(fieldValue[0], compare[0])) &&
						((fieldValue[1] > compare[1]) || float.AlmostEqual(fieldValue[1], compare[1])) &&
						((fieldValue[2] > compare[2]) || float.AlmostEqual(fieldValue[2], compare[2]))) return true;
				}

				return false;
			}
		}

		return false;
	}
}
