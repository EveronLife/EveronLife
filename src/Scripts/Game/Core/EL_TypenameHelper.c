class EL_TypenameHelper
{
	static int GetVariableIndexForField(Class instance, string fieldName)
	{
		typename type = instance.Type();
		
		for(int nVariable = 0; nVariable < type.GetVariableCount(); nVariable++)
		{
			string name = type.GetVariableName(nVariable);
			
			//Type has no variables
			if(!name) break;
			
			if(name == fieldName)
			{
				return nVariable;
			}
		}
		
		return -1;
	}
	
	static typename GetVariableType(Class instance, string fieldName)
	{
		return instance.Type().GetVariableType(GetVariableIndexForField(instance, fieldName)); 
	}
}
