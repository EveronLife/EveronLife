class EL_DbFindConditionEvaluator
{
	static array<ref EL_DbEntity> GetFiltered(notnull array<ref EL_DbEntity> entities, notnull EL_DbFindCondition condition)
	{
		array<ref EL_DbEntity> conditionMatched();
		
		foreach(EL_DbEntity entity : entities)
		{
			if(EL_DbFindConditionEvaluator.Matches(entity, condition))
			{
				conditionMatched.Insert(entity);
			}
		}
		
		return conditionMatched;
	}
	
	static bool Matches(notnull EL_DbEntity entity, notnull EL_DbFindCondition condition)
	{
		switch(condition.Type())
		{
			case EL_DbFindAnd:
			{
				EL_DbFindAnd andCondition = EL_DbFindAnd.Cast(condition);
				
				foreach(EL_DbFindCondition checkCondition : andCondition.m_Conditions)
				{
					if(!Matches(entity, checkCondition)) return false;
				}
				
				return true;
			}
			
			case EL_DbFindOr:
			{
				EL_DbFindOr orCondition = EL_DbFindOr.Cast(condition);
				
				foreach(EL_DbFindCondition checkCondition : orCondition.m_Conditions)
				{
					if(Matches(entity, checkCondition)) return true;
				}
				
				return false;
			}
			
			case EL_DbFindCompareFieldInt:
			{
				EL_DbFindCompareFieldInt compareIntConditon = EL_DbFindCompareFieldInt.Cast(condition);
				
				int fieldValue;
				if(!EL_DotNotationReader<int>.ReadValue(entity, compareIntConditon.m_FieldName, fieldValue)) return false;
				
				switch(compareIntConditon.m_ComparisonOperator)
				{
					case EL_DbNumericFieldOperator.EQUAL: return (fieldValue == compareIntConditon.m_ComparisonValue);
					case EL_DbNumericFieldOperator.NOT_EQUAL: return (fieldValue != compareIntConditon.m_ComparisonValue);
					case EL_DbNumericFieldOperator.LESS_THAN: return (fieldValue < compareIntConditon.m_ComparisonValue);
					case EL_DbNumericFieldOperator.LESS_THAN_OR_EQUAL: return (fieldValue <= compareIntConditon.m_ComparisonValue);
					case EL_DbNumericFieldOperator.GREATER_THAN: return (fieldValue > compareIntConditon.m_ComparisonValue);
					case EL_DbNumericFieldOperator.GREATER_THAN_OR_EQUAL: return (fieldValue >= compareIntConditon.m_ComparisonValue);
				}
				
				return false;
			}
			
			case EL_DbFindCompareFieldFloat:
			{
				EL_DbFindCompareFieldFloat compareFloatConditon = EL_DbFindCompareFieldFloat.Cast(condition);
				
				float fieldValue;
				if(!EL_DotNotationReader<float>.ReadValue(entity, compareFloatConditon.m_FieldName, fieldValue)) return false;
				
				//Using AlmostEqual to avoid floating point issues. For <= / >= we check if the native operator already says its true and only if not then we double check for equal
				switch(compareFloatConditon.m_ComparisonOperator)
				{
					case EL_DbNumericFieldOperator.EQUAL: return (float.AlmostEqual(fieldValue, compareFloatConditon.m_ComparisonValue));
					case EL_DbNumericFieldOperator.NOT_EQUAL: return (!float.AlmostEqual(fieldValue, compareFloatConditon.m_ComparisonValue));
					case EL_DbNumericFieldOperator.LESS_THAN: return (fieldValue < compareFloatConditon.m_ComparisonValue);
					case EL_DbNumericFieldOperator.LESS_THAN_OR_EQUAL: return ((fieldValue <= compareFloatConditon.m_ComparisonValue) || float.AlmostEqual(fieldValue, compareFloatConditon.m_ComparisonValue));
					case EL_DbNumericFieldOperator.GREATER_THAN: return (fieldValue > compareFloatConditon.m_ComparisonValue);
					case EL_DbNumericFieldOperator.GREATER_THAN_OR_EQUAL: return ((fieldValue >= compareFloatConditon.m_ComparisonValue) || float.AlmostEqual(fieldValue, compareFloatConditon.m_ComparisonValue));
				}
				
				return false;
			}
			
			case EL_DbFindCompareFieldBool:
			{
				EL_DbFindCompareFieldBool compareBoolConditon = EL_DbFindCompareFieldBool.Cast(condition);
				
				bool fieldValue;
				if(!EL_DotNotationReader<bool>.ReadValue(entity, compareBoolConditon.m_FieldName, fieldValue)) return false;
				
				return fieldValue == compareBoolConditon.m_ComparisonValue;
			}
			
			case EL_DbFindCompareFieldString:
			{
				EL_DbFindCompareFieldString compareStringConditon = EL_DbFindCompareFieldString.Cast(condition);
				
				string fieldValue;
				if(!EL_DotNotationReader<string>.ReadValue(entity, compareStringConditon.m_FieldName, fieldValue)) return false;
				
				switch(compareStringConditon.m_ComparisonOperator)
				{
					case EL_DbStringFieldOperator.EQUAL: return (fieldValue == compareStringConditon.m_ComparisonValue);
					case EL_DbStringFieldOperator.NOT_EQUAL: return (fieldValue != compareStringConditon.m_ComparisonValue);
					case EL_DbStringFieldOperator.CONTAINS: return (fieldValue.Contains(compareStringConditon.m_ComparisonValue));
					case EL_DbStringFieldOperator.NOT_CONTAINS: return (!fieldValue.Contains(compareStringConditon.m_ComparisonValue));
				}
				
				return false;
			}
		}
		
		return false;
	}
}
