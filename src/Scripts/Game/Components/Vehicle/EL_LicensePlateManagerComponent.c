[EntityEditorProps(category: "EL/LicnesePlateManagerComponent", description: "This renders license plates on your vehicle.", color: "0 0 255 255")]
class EL_LicensePlateManagerComponentClass: ScriptComponentClass
{
};

class EL_LicensePlatePointInfo: PointInfo
{	
	EL_LicensePlateEntity m_Object;
	
	void ~EL_LicensePlatePointInfo()
	{
		if (m_Object)
		{
			delete m_Object;
		}
	}
}

class EL_LicensePlateManagerComponent: ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.Auto)]
	ref array<ref EL_LicensePlatePointInfo> m_Plates;
	
	[Attribute("{E95486C43308F36B}Prefabs/Vehicles/LicensePlate/LicensePlate.et")]
	protected ResourceName m_LicensePlatePrefab;
	
	[RplProp(onRplName: "OnRegistrationUpdated")]
	string m_Registration;
	
	override void EOnInit(IEntity owner)
	{
		Resource resource = Resource.Load(m_LicensePlatePrefab);
		
		if (!resource.IsValid()) return;
		
		EntitySpawnParams params();
		params.Parent = owner;
		params.TransformMode = ETransformMode.LOCAL;
		
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		if (GetGame().InPlayMode() && rpl && rpl.IsMaster())
		{
			Resource container = BaseContainerTools.LoadContainer("{B1DD7B5D4812AB19}Configs/Vehicles/VehicleSettings.conf");
			EL_VehicleSettings vehicleSettings = EL_VehicleSettings.Cast(BaseContainerTools.CreateInstanceFromContainer(container.GetResource().ToBaseContainer()));
			m_Registration = vehicleSettings.m_LicensePlateGenerator.GenerateLicensePlate();
		}
		
		for (int i = 0; i < m_Plates.Count(); i++)
		{
			auto plate = m_Plates[i];
			GetPositionFromPoint(i, params.Transform);

			plate.m_Object = EL_LicensePlateEntity.Cast(GetGame().SpawnEntityPrefabLocal(resource, owner.GetWorld(), params));
			Vehicle veh = Vehicle.Cast(owner);
			
			veh.AddChild(plate.m_Object, -1, EAddChildFlags.RECALC_LOCAL_TRANSFORM);
			plate.m_Object.m_LicensePlateManager = this;
		}
	}
	
	void OnRegistrationUpdated()
	{
		foreach (auto plate : m_Plates)
		{
			plate.m_Object.m_TextWidget.SetText(m_Registration);
		}
	}
	
	override void OnPostInit(IEntity owner)
	{		
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, false);
	}
	
	bool GetPositionFromPoint(int index, out vector transform[4])
	{
		Math3D.MatrixIdentity4(transform);
		BaseContainer source;
		GenericEntity entity = GetOwner();
		BaseContainerList list;
		
		source = GetComponentSource(entity);
		if (!source) return false;
		
		list = source.GetObjectArray("m_Plates");
		if (!list) return false;

		source = list.Get(index);
		if (!source) return false;

		vector position;
		source.Get("Offset", position);
		
		vector rotation;
		source.Get("Angles", rotation);

		transform[3] = position;
		
		Math3D.AnglesToMatrix(rotation, transform);

		return true;
	}
};
