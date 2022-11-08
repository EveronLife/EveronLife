class EL_GarageData
{
	ResourceName m_rPrefab;
	int m_iVehicleColor;
	
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet) 
	{

		snapshot.Serialize(packet, 26);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{
		return snapshot.Serialize(packet, 26);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx) 
	{	
		return lhs.CompareSnapshots(rhs, 26);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool PropCompare(EL_GarageData prop, SSnapSerializerBase snapshot, ScriptCtx ctx) 
	{
		return snapshot.Compare(prop.m_rPrefab, 22)
			&& snapshot.CompareInt(prop.m_iVehicleColor);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(EL_GarageData prop, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{	
		snapshot.SerializeBytes(prop.m_rPrefab, 22);
		snapshot.SerializeInt(prop.m_iVehicleColor);
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, EL_GarageData prop) 
	{
		snapshot.SerializeBytes(prop.m_rPrefab, 22);
		snapshot.SerializeInt(prop.m_iVehicleColor);
		
		return true;
	}
}