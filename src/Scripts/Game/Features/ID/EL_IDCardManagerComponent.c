[EntityEditorProps(category: "EL/IDCardManagerComponent", description: "This renders the ID Card", color: "0 0 255 255")]
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
	ref array<ref EL_IDCardPointInfo> m_Plates;
	
	[Attribute("{C5EDD08670904FBD}Prefabs/Items/Card/Id/ID_Card.et")]
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
		
		if (GetGame().InPlayMode() && rpl && rpl.IsMaster())
		{
			Resource container = BaseContainerTools.LoadContainer("{B1DD7B5D4812AB19}Configs/Vehicles/IDCardSettings.conf");
			EL_IDCardSettings idcardSettings = EL_IDCardSettings.Cast(BaseContainerTools.CreateInstanceFromContainer(container.GetResource().ToBaseContainer()));
			m_Registration = idcardSettings.m_IDCardGenerator.GenerateIDCard();
		}
	}
}