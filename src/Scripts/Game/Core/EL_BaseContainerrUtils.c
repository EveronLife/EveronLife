class EL_BaseContainerUtils
{
	//------------------------------------------------------------------------------------------------
	static void DumpVariables(notnull BaseContainer baseContainer)
	{
		PrintFormat("Dumping variables of '%1::%2':", baseContainer.GetName(), baseContainer.GetClassName());
		for (int nVar = 0, count = baseContainer.GetNumVars(); nVar < count; nVar++)
		{
			PrintFormat("[%1] %2:%3", nVar, baseContainer.GetVarName(nVar), typename.EnumToString(DataVarType, baseContainer.GetDataVarType(nVar)))
		}
	}
};
