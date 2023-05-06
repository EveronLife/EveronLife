# New vehicle shop
A vehicle shop needs an Entity with the `EL_VehicleShopManagerComponent` to open the vehicle shop.
This Entity needs to be a child of another Entity e.g. a building. 
The Building needs to have one or more spawn points (Prefab: `EL_VehicleSpawnPoint`)

# Preview Building
A preview building (e.g `Prefabs/Buildings/VehicleShop/VEHICLE_SHOP_PREVIEW.et`) needs to be placed in the world (Best to put far away. e.g pos: 0,100,0). <br />
You need to set a name in the object properties on the top right.<br />
At least one is required for all vehicle shops, but multiple can be added with different names

## EL_VehicleShopManagerComponent
Variables:<br />
**Shop Preview Building Name** -> The name of the preview building where the vehicle previews are spawned<br />
**Empty Vehicle Preview** -> The prefab for the empty preview<br />
**Vehicle Shop Camera** -> The prefab for the preview building camera<br />
**Camera Point/Angles** -> The pos / rotation for the camera in the building (To face the preview vehicle)<br />

## Vehicle price config
A config of the <EL_PriceConfig> Type needs to be set here.


## Vehicle Spawn Point Prefab
The trigger box size can be changed but the Entity itself needs to be **outside** the box to not falsely trigger it. (e.g Box Mins Y:-0.5)

