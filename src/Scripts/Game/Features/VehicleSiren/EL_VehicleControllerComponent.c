class EL_VehicleControllerComponentClass: CompartmentControllerComponentClass
{
};

class EL_VehicleControllerComponent: CompartmentControllerComponent
{
	proto external bool StartSiren();
	proto external void StopSiren();
	
	proto external bool IsSirenOn();
};

