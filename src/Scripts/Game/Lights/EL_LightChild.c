[BaseContainerProps()]
class EL_LightChild
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, params: "et", desc: "Prefab of the LightEntity that will be used. Can be left blank")]
	protected ResourceName m_LightPrefab;
	
	[Attribute("90", desc: "Used to circumvent a game bug", params: "0 360")]
	protected float m_ConeAngle;
	
	[Attribute(desc: "Position of the light relative to parent root or bone")]
	protected ref PivotPoint m_PivotPoint;
	
	
	protected LightEntity m_LightEntity;
	
	protected ref Resource m_LightResource;
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetPrefab()
	{
		return m_LightPrefab;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Loads the resource into memory, spawns the entity and sets it as a child of parent
	//! \param parent The parent of the spwaned light
	//! \return If it could load the resource
	bool Spawn(IEntity parent)
	{
		m_LightEntity = LightEntity.Cast(EL_Utils.SpawnAsChild(m_LightPrefab, parent, m_PivotPoint));
		if(!m_LightEntity) return false;
		m_LightEntity.SetConeAngle(m_ConeAngle);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void Destroy()
	{
		delete m_LightEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~EL_LightChild()
	{
		Destroy();
	}
}