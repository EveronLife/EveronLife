class EL_SirenControllerComponentClass: EL_VehicleControllerComponentClass
{
};


class EL_SirenControllerComponent: EL_VehicleControllerComponent
{
	event void OnSirenStart();
	event void OnSirenStop();
	event void OnPostInit(IEntity owner);
	event void OnDelete(IEntity owner);
}