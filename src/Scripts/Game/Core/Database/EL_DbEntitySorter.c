class EL_DbEntitySorter
{
	static array<ref EL_DbEntity> GetSorted(notnull array<ref EL_DbEntity> entities, notnull EL_TStringArrayArray orderBy, int orderByIndex = 0)
	{
		if(entities.Count() == 0 || orderByIndex >= orderBy.Count()) return entities;
		
		string fieldName = orderBy.Get(orderByIndex).Get(0);
		string sortDirection = orderBy.Get(orderByIndex).Get(1);
		sortDirection.ToLower();
		bool descending = sortDirection == "desc";
		
		//Collect all values for the field being sorted
		map<string, ref array<ref EL_DbEntity>> distinctValues();
		
		//Get type from first instance, they should all be the same
		typename fieldType = EL_TypenameHelper.GetVariableType(entities.Get(0), fieldName);
		
		//Add each entity to distinct map of field values
		foreach(EL_DbEntity entity : entities)
		{
			string valueKey;
			
			if(fieldType.IsInherited(bool))
			{
				bool fieldValue;
				EL_DotNotationReader<bool>.ReadValue(entity, fieldName, fieldValue);
				valueKey = fieldValue.ToString();
			}
			else if(fieldType.IsInherited(int))
			{
				int fieldValue;
				EL_DotNotationReader<int>.ReadValue(entity, fieldName, fieldValue);
				valueKey = fieldValue.ToString();
			}
			else if(fieldType.IsInherited(float))
			{
				float fieldValue;
				EL_DotNotationReader<float>.ReadValue(entity, fieldName, fieldValue);
				valueKey = fieldValue.ToString();
			}
			else if(fieldType.IsInherited(string))
			{
				EL_DotNotationReader<string>.ReadValue(entity, fieldName, valueKey);
			}
			else
			{
				Debug.Error(string.Format("Can not sort entity collection by field '%1' with non sortable type '%2'", fieldName, fieldType));
				return entities;
			}
			
			array<ref EL_DbEntity> entityArray = distinctValues.Get(valueKey);
			if(!entityArray)
			{
				entityArray = new array<ref EL_DbEntity>();
				distinctValues.Set(valueKey, entityArray);
			}
			
			entityArray.Insert(entity);
		}
		
		//Sort all field values according to input
		array<string> valueKeysSorted();
		valueKeysSorted.Resize(distinctValues.Count());
		
		if(fieldType.IsInherited(bool))
		{
			array<bool> valueKeysSortedTyped();
			valueKeysSortedTyped.Resize(distinctValues.Count());
			
			//Parse strings back into sortable booleans
			for(int nKey = 0; nKey < distinctValues.Count(); nKey++)
			{
				valueKeysSortedTyped.Set(nKey, distinctValues.GetKey(nKey) == "true");
			}
			
			//Sort booleans
			valueKeysSortedTyped.Sort(descending);
			
			//Turn back into strings to get objects from map
			foreach(int index, bool sortedKey : valueKeysSortedTyped)
			{
				valueKeysSorted.Set(index, sortedKey.ToString());
			}
		}
		else if(fieldType.IsInherited(int))
		{
			array<int> valueKeysSortedTyped();
			valueKeysSortedTyped.Resize(distinctValues.Count());
			
			//Parse strings back into sortable integers
			for(int nKey = 0; nKey < distinctValues.Count(); nKey++)
			{
				valueKeysSortedTyped.Set(nKey, distinctValues.GetKey(nKey).ToInt());
			}
			
			//Sort integers
			valueKeysSortedTyped.Sort(descending);
			
			//Turn back into strings to get objects from map
			foreach(int index, int sortedKey : valueKeysSortedTyped)
			{
				valueKeysSorted.Set(index, sortedKey.ToString());
			}
		}
		else if(fieldType.IsInherited(float))
		{
			array<float> valueKeysSortedTyped();
			valueKeysSortedTyped.Resize(distinctValues.Count());
			
			//Parse strings back into sortable floats
			for(int nKey = 0; nKey < distinctValues.Count(); nKey++)
			{
				valueKeysSortedTyped.Set(nKey, distinctValues.GetKey(nKey).ToFloat());
			}
			
			//Sort floats
			valueKeysSortedTyped.Sort(descending);
			
			//Turn back into strings to get objects from map
			foreach(int index, float sortedKey : valueKeysSortedTyped)
			{
				valueKeysSorted.Set(index, sortedKey.ToString());
			}
		}
		else if(fieldType.IsInherited(string))
		{
			for(int nKey = 0; nKey < distinctValues.Count(); nKey++)
			{
				valueKeysSorted.Set(nKey, distinctValues.GetKey(nKey));
			}
			valueKeysSorted.Sort(descending);
		}
		
		int nSortedEntity = 0;
		array<ref EL_DbEntity> sortedEnties();
		sortedEnties.Resize(entities.Count());
		
		foreach(string sortedValueKey : valueKeysSorted)
		{
			array<ref EL_DbEntity> sameKeyEntities = distinctValues.Get(sortedValueKey);
			
			if(sameKeyEntities.Count() == 1)
			{
				sortedEnties.Set(nSortedEntity++, sameKeyEntities.Get(0));
			}
			else
			{
				array<ref EL_DbEntity> subSortedEnities = GetSorted(sameKeyEntities, orderBy, orderByIndex + 1);
				
				foreach(EL_DbEntity subSortedEntity : subSortedEnities)
				{
					sortedEnties.Set(nSortedEntity++, subSortedEntity);
				}
			}
		}
		
		return sortedEnties;
	}
}
