/*[EntityEditorProps(category: "EL/IDCardManagerComponent", description: "This renders the ID Card", color: "0 0 255 255")]
class EL_IDCardManagerComponentClass: ScriptComponentClass
{
};

class EL_IDCardPointInfo: PointInfo
{	
	EL_IDCardEntity m_Object;
	
	void ~EL_IDCardPointInfo()
	{
		if (m_Object)
		{
			delete m_Object;
		}
	}
}

class EL_IDCardManagerComponent: ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.Auto)]
	ref array<ref EL_IDCardPointInfo> m_Licenses;
	
	[Attribute("{6784EF9BC821D725}Prefabs/Items/Equipment/Accessories/IdCard/IDCard.et")]
	protected ResourceName m_IDCardPrefab;
	
	[RplProp(onRplName: "OnRegistrationUpdated")]
	string m_Registration;
	
	override void EOnInit(IEntity owner)
	{
		Resource resource = Resource.Load(m_IDCardPrefab);
		
		if (!resource.IsValid()) return;
		
		EntitySpawnParams params();
		params.Parent = owner;
		params.TransformMode = ETransformMode.LOCAL;
		
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		for (int i = 0; i < m_Licenses.Count(); i++)
		{
			auto licence = m_Licenses[i];

			licence.m_Object = EL_IDCardEntity.Cast(GetGame().SpawnEntityPrefabLocal(resource, owner.GetWorld(), params));
			
			licence.m_Object.m_IDCardManager = this;
		}
	}
	
	void OnRegistrationUpdated()
	{
		foreach (auto licence : m_Licenses)
		{
			licence.m_Object.m_TextWidget.SetText(m_Registration);
		}
			
	}
}/*