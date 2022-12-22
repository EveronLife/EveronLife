[EntityEditorProps(category: "EL/Vehicle")]
class EL_VehicleAppearanceComponentClass : ScriptComponentClass
{
};

class EL_VehicleAppearanceComponent : ScriptComponent
{
	[RplProp(onRplName: "OnVehicleColorChanged")]
	protected int m_iVehicleColor = -1;
	
	[RplProp(onRplName: "OnVehicleTextureChanged")]
	protected ResourceName m_VehicleTexture;
	
	
	//------------------------------------------------------------------------------------------------
	int GetVehicleColor()
	{
		return m_iVehicleColor;
	}
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetVehicleTexture()
	{
		return m_VehicleTexture;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetVehicleColor(int color)
	{
		if (m_iVehicleColor == color)
			return;
		
		m_iVehicleColor = color;
		
		OnVehicleColorChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetVehicleTexture(ResourceName texture)
	{
		if (m_VehicleTexture == texture)
			return;
		
		m_VehicleTexture = texture;
		
		OnVehicleTextureChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnVehicleColorChanged()
	{
		EL_Utils.SetColor(GetOwner(), Color.FromInt(m_iVehicleColor));
		EL_Utils.SetSlotsColor(GetOwner(), m_iVehicleColor);
	}
	
	//------------------------------------------------------------------------------------------------
	//! TODO: Add textures
	void OnVehicleTextureChanged()
	{
		
	}
	
}