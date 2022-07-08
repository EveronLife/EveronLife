class EL_DotNotationReader<Class T>
{
	// TODO: Add cache for variable index and typename? Do benchmark if that is faster than typename access.
	
	static bool ReadValue(notnull Class instance, string fieldName, out T resultValue)
	{
		if(fieldName.Contains("."))
		{
			Debug.Error("Accessing sub fields via toplevel.childField is not yet supported.");
			return false;
		}
		
		int vIdx = EL_TypenameHelper.GetVariableIndexForField(instance, fieldName);
		if(vIdx == -1)
		{
			Debug.Error(string.Format("Failed to read field '%1' that is not present on '%2'.", fieldName, instance));
			return false;
		}

		if(!instance.Type().GetVariableValue(instance, vIdx, resultValue)) 
		{
			Debug.Error(string.Format("Failed to read field '%1' as '%2' from the '%3'.", fieldName, T, instance));
			return false;
		}
		
		return true;
	}
}
