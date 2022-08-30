class EL_DbEntitySortDirection
{
	static const string ASCENDING = "ASC";
	static const string DESCENDING = "DESC";
}

class EL_DbEntitySorter
{
	//------------------------------------------------------------------------------------------------
	static array<ref EL_DbEntity> GetSorted(notnull array<ref EL_DbEntity> entities, notnull array<ref TStringArray> orderBy, int orderByIndex = 0)
	{
		if (entities.Count() < 2 || orderByIndex >= orderBy.Count()) return entities;

		string fieldName = orderBy.Get(orderByIndex).Get(0);
		array<string> fieldSplits();
		fieldName.Split(".", fieldSplits, true);

		// Empty sort condition, nothing to do
		if (fieldSplits.IsEmpty()) return entities;

		string sortDirection = orderBy.Get(orderByIndex).Get(1);
		sortDirection.ToLower();
		bool descending = sortDirection == "desc";

		//Collect all values for the field being sorted
		map<string, ref array<ref EL_DbEntity>> distinctValues();

		//Get type from first instance, they should all be the same
		typename fieldType;

		//Add each entity to distinct map of field values
		foreach (EL_DbEntity entity : entities)
		{
			string valueKey;
			GetSortValue(entity, fieldSplits, 0, valueKey, fieldType);

			array<ref EL_DbEntity> entityArray = distinctValues.Get(valueKey);
			if (!entityArray)
			{
				entityArray = new array<ref EL_DbEntity>();
				distinctValues.Set(valueKey, entityArray);
			}

			entityArray.Insert(entity);
		}

		//Sort all field values according to input
		array<string> valueKeysSorted();
		valueKeysSorted.Resize(distinctValues.Count());

		// TODO: Use fixed lengh padded numbers to rely only on string sort without conversion
		switch(fieldType)
		{
			case int:
			{
				array<int> valueKeysSortedTyped();
				valueKeysSortedTyped.Resize(distinctValues.Count());

				//Parse strings back into sortable integers
				for (int nKey = 0; nKey < distinctValues.Count(); nKey++)
				{
					valueKeysSortedTyped.Set(nKey, distinctValues.GetKey(nKey).ToInt());
				}

				//Sort integers
				valueKeysSortedTyped.Sort(descending);

				//Turn back into strings to get objects from map
				foreach (int index, int sortedKey : valueKeysSortedTyped)
				{
					valueKeysSorted.Set(index, sortedKey.ToString());
				}

				break;
			}

			case float:
			{
				array<float> valueKeysSortedTyped();
				valueKeysSortedTyped.Resize(distinctValues.Count());

				//Parse strings back into sortable floats
				for (int nKey = 0; nKey < distinctValues.Count(); nKey++)
				{
					valueKeysSortedTyped.Set(nKey, distinctValues.GetKey(nKey).ToFloat());
				}

				//Sort floats
				valueKeysSortedTyped.Sort(descending);

				//Turn back into strings to get objects from map
				foreach (int index, float sortedKey : valueKeysSortedTyped)
				{
					valueKeysSorted.Set(index, sortedKey.ToString());
				}

				break;
			}

			case bool:
			{
				array<bool> valueKeysSortedTyped();
				valueKeysSortedTyped.Resize(distinctValues.Count());

				//Parse strings back into sortable booleans
				for (int nKey = 0; nKey < distinctValues.Count(); nKey++)
				{
					valueKeysSortedTyped.Set(nKey, distinctValues.GetKey(nKey) == "true");
				}

				//Sort booleans
				valueKeysSortedTyped.Sort(descending);

				//Turn back into strings to get objects from map
				foreach (int index, bool sortedKey : valueKeysSortedTyped)
				{
					valueKeysSorted.Set(index, sortedKey.ToString());
				}

				break;
			}

			case string:
			{
				for (int nKey = 0; nKey < distinctValues.Count(); nKey++)
				{
					valueKeysSorted.Set(nKey, distinctValues.GetKey(nKey));
				}
				valueKeysSorted.Sort(descending);

				break;
			}

			case vector:
			{
				array<vector> valueKeysSortedTyped();
				valueKeysSortedTyped.Resize(distinctValues.Count());

				//Parse strings back into sortable booleans
				for (int nKey = 0; nKey < distinctValues.Count(); nKey++)
				{
					valueKeysSortedTyped.Set(nKey, distinctValues.GetKey(nKey).ToVector());
				}

				//Sort booleans
				valueKeysSortedTyped.Sort(descending);

				//Turn back into strings to get objects from map
				foreach (int index, vector sortedKey : valueKeysSortedTyped)
				{
					valueKeysSorted.Set(index, sortedKey.ToString(false));
				}

				break;
			}
		}

		int nSortedEntity = 0;
		array<ref EL_DbEntity> sortedEnties();
		sortedEnties.Resize(entities.Count());

		foreach (string sortedValueKey : valueKeysSorted)
		{
			array<ref EL_DbEntity> sameKeyEntities = distinctValues.Get(sortedValueKey);

			if (sameKeyEntities.Count() == 1)
			{
				sortedEnties.Set(nSortedEntity++, sameKeyEntities.Get(0));
			}
			else
			{
				array<ref EL_DbEntity> subSortedEnities = GetSorted(sameKeyEntities, orderBy, orderByIndex + 1);

				foreach (EL_DbEntity subSortedEntity : subSortedEnities)
				{
					sortedEnties.Set(nSortedEntity++, subSortedEntity);
				}
			}
		}

		return sortedEnties;
	}

	//------------------------------------------------------------------------------------------------
	protected static void GetSortValue(Class instance, array<string> fieldSplits, int currentIndex, out string sortValue, out typename valueType)
	{
		string currentFieldName = fieldSplits.Get(currentIndex);
		typename type = instance.Type();
		int vIdx = GetVIdx(type, currentFieldName);
		valueType = type.GetVariableType(vIdx);

		// Expand nested object
		if (currentIndex < fieldSplits.Count() - 1)
		{
			if (valueType.IsInherited(array) || valueType.IsInherited(set) || valueType.IsInherited(map))
			{
				Debug.Error(string.Format("Can not get sort value from collection type '%1' on '%2.%3'", valueType, type, currentFieldName));
				return;
			}
			else if (!valueType.IsInherited(Class))
			{
				Debug.Error(string.Format("Can not expand primitive type '%1' on '%2.%3' to read field '%4'", valueType, type, currentFieldName, fieldSplits.Get(currentIndex + 1)));
				return;
			}

			Class complexHolder;
			if (!type.GetVariableValue(instance, vIdx, complexHolder)) return;
			GetSortValue(complexHolder, fieldSplits, currentIndex + 1, sortValue, valueType);
		}

		// Read primitive field value and convert to compareable
		switch(valueType)
		{
			case int:
			{
				int outVal;
				if (!type.GetVariableValue(instance, vIdx, outVal)) return;
				sortValue = outVal.ToString();
				return;
			}

			case float:
			{
				float outVal;
				if (!type.GetVariableValue(instance, vIdx, outVal)) return;
				sortValue = outVal.ToString();
				return;
			}

			case bool:
			{
				bool outVal;
				if (!type.GetVariableValue(instance, vIdx, outVal)) return;
				sortValue = outVal.ToString();
				return;
			}

			case string:
			{
				if (!type.GetVariableValue(instance, vIdx, sortValue)) return;
				return;
			}

			case vector:
			{
				vector outVal;
				if (!type.GetVariableValue(instance, vIdx, outVal)) return;
				sortValue = outVal.ToString(false);
				return;
			}
		}

		Debug.Error(string.Format("Can not sort entity collection by field '%1' with non sortable type '%2'.", currentFieldName, valueType));
	}

	//------------------------------------------------------------------------------------------------
	protected static int GetVIdx(typename type, string fieldName)
	{
		for (int vIdx = 0; vIdx < type.GetVariableCount(); vIdx++)
		{
			string variableName = type.GetVariableName(vIdx);

			if (!variableName) break;

			if (variableName == fieldName) return vIdx;
		}

		return -1;
	}
}
