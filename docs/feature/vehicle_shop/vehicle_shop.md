# New vehicle shop
A vehicle shop needs an Entity with the <EL_VehicleShopManagerComponent> to open the vehicle shop.
This Entity needs to be a child of another Entity e.g. a building. 
The Building needs to have one or more spawn points (Prefab: <EL_VehicleSpawnPoint>)

## EL_VehicleShopManagerComponent
Variables:
Shop Preview Building Name -> The name of the preview building where the vehicle previews are spawned (Best to put far away. like 0,100,0)
Empty Vehicle Preview -> The prefab for the empty preview
Vehicle Shop Camera -> The prefab for the preview building camera
Camera Point/Angles -> The pos / rotation for the camera in the building (To face the preview vehicle)

## Vehicle price config
A config of the <EL_PriceConfig> Type needs to be set here.


## Vehicle Spawn Point Prefab
The trigger box size can be changed but the Entity itself needs to be **outside** the box to not falsely trigger it. (e.g Box Mins Y:-0.5)

